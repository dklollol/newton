#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <libplayerc++/playerc++.h>

#include "misc.h"
#include "draw.h"
#include "robot.h"
#include "camera.h"
#include "particles.h"
#include "random_numbers.h"
#include "strategy.h"
#include "timing.h"
#include "green-partition/cam.h"

using namespace std;
using namespace cv;
using namespace PlayerCc;


// Global main loop info.
bool is_done = false;

// This is all just a hack to get around the problem that
// `cam.set(CV_CAP_PROP_BUFFERSIZE, 1);` does not work for our webcam.  Instead
// we continually grab camera frames in a seperate thread, and in that way
// always read the newest frame.
mutex camera_mutex;
condition_variable camera_condition_variable;
bool camera_is_grabbing;
Mat camera_current_frame;
std::vector<Box> boxes;

void grab_from_camera(camera cam) {
  unique_lock<mutex> camera_lock(camera_mutex, defer_lock);

  while (true) {
    camera_lock.lock();
    if (is_done) {
      break;
    }
    else if (camera_is_grabbing) {
      camera_current_frame = cam.get_colour();
    }
    else {
      camera_condition_variable.wait(camera_lock,
                                     []{ return camera_is_grabbing; });
    }
    camera_lock.unlock();
  }
}

Mat get_newest_camera_frame() {
  unique_lock<mutex> camera_lock(camera_mutex, defer_lock);
  Mat frame;

  camera_is_grabbing = false;
  camera_lock.lock();
  frame = camera_current_frame;
  camera_lock.unlock();
  camera_is_grabbing = true;
  camera_condition_variable.notify_one();
  return frame;
}

void run(char* host, int port, int device_index) {
  // Constants
  double world_width = 500.0;
  double world_height = 500.0;

  // The GUI
  const char *map = "World map";
  const char *window = "Robot view";
  namedWindow(map, CV_WINDOW_AUTOSIZE);
  namedWindow(window, CV_WINDOW_AUTOSIZE);
  moveWindow(window, 500.0, 20.0);

  // Picture matrices
  Mat world(cvSize (world_width, world_height), CV_8UC3);
  Mat im;

  // Initialize particles.
  const int num_particles = 2000;
  vector<particle> particles(num_particles);


  // Set initial particle values.
  const double offset = 100.0;
  for (int i = 0; i < num_particles; i++) {
    // Random starting points. (x,y) \in [-400, 400]^2, theta \in [-pi, pi].
    particles[i].x = world_width * randf() - offset;
    particles[i].y = world_height * randf() - offset;
    particles[i].theta = 2.0 * M_PI * randf() - M_PI;
    particles[i].weight = 1.0 / (double) num_particles;
  }

  // Setup the camera interface.
  camera cam(0, Size(640, 480), false);
  camera_is_grabbing = true;
  camera_current_frame = cam.get_colour();
  thread camera_thread(grab_from_camera, cam);

  // Initialize player.
  PlayerClient robot(host, port);
  set_pull_mode(robot);
  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, device_index);

  // States
  driving_state_t driving_state = searching_random;
  pos_t pos;
  object::type landmark_id;
  double measured_distance;
  double measured_angle;
  colour_prop cp;
  particle est_pose;

  // Main loop.
  while (true) {
    puts(""); // Empty line.

    TIMER_START();

    // Wait a little.
    int action = cvWaitKey(4);
    char action_char = (char) action;
    // switch (action_char) {
    // case 'q': // Quit
    //   goto after_loop;
    //   break;
    // }
    if (driving_state == finished) {
      goto after_loop;
    }
    // Grab image.
    TIMER_START();
    get_newest_camera_frame().copyTo(im);
    TIMER_END("Read from camera");

    // Get green boxes.
    TIMER_START();
    boxes = process_vertical_lines(im);
    TIMER_END("Found green boxes");

    // Do landmark detection.
    TIMER_START();
    landmark_id = cam.get_object(im, cp, measured_distance, measured_angle);
    printf("[LANDMARK DETECTION] %s\n", object::name(landmark_id).c_str());
    TIMER_END("Locate landmark");

    // if (landmark_id != object::none) {
      printf("[MEASUREMENTS] Distance: %.3lf, angle: %.3lf, rgb: (%.3lf, %.3lf, %.3lf)\n",
             measured_distance, measured_angle, cp.red, cp.green, cp.blue);
      //}

    // Correction step: Compute particle weights.
    TIMER_START();
    calculate_weights(&particles, measured_distance, measured_angle,
                        landmark_id);

    TIMER_END("Set particle weights");

    // Resampling step.
    TIMER_START();
    resample(&particles);
    TIMER_END("Resample particles");

    // Driving step.
    TIMER_START();
    pos.x = 0.0;
    pos.y = 0.0;
    pos.turn = 0.0;
    printf("State before execute: %s\n", stateMap[driving_state].c_str());
    execute_strategy(robot, pp, ir, pos, est_pose, driving_state, landmark_id,
                     measured_distance, measured_angle);
    printf("[ESTIMATE] Relative change: x: %.3lf, y: %.3lf, turn: %.3lf\n",
           pos.x, pos.y, radians_to_degrees(pos.turn));
    TIMER_END("Execute strategy");

    // Prediction step: Update all particles according to how much we have
    // moved.
    TIMER_START();
    for (int i = 0; i < num_particles; i++) {
      move_particle(particles[i], pos.x, pos.y, pos.turn);
    }
    TIMER_END("Move particles");

    // Add uncertainty.
    TIMER_START();
    double driveVar;
    double turnVar;
    tie(driveVar, turnVar) = command_variance(&pos);
    printf("[NOISE ADDITION] dist: %.3lf, turn: %.3lf\n", driveVar, turnVar);
    add_uncertainty(particles, driveVar, turnVar);
    TIMER_END("Add uncertainty");

    // Estimate pose.
    TIMER_START();
    est_pose = estimate_pose(particles);
    printf("[ESTIMATE] Current location: x: %.3lf, y: %.3lf, theta: %.3lf\n",
           est_pose.x, est_pose.y, radians_to_degrees(est_pose.theta));
    TIMER_END("Estimate pose");

    // Draw the landmark in the image if found.
    TIMER_START();
    cam.draw_object(im);
    TIMER_END("Draw landmark");

    // Draw particle world.
    TIMER_START();
    draw_world(est_pose, particles, world);
    TIMER_END("Draw particle world");

    // Render windows.
    TIMER_START();
    imshow(map, world);
    imshow(window, im);
    TIMER_END("Render windows");

    TIMER_END("Run one iteration");
 }

 after_loop:
  is_done = true;
  camera_thread.join();
  pp.SetSpeed(0.0, 0.0);
}

int main(int argc, char* argv[]) {
  char* host;
  if (argc > 1) {
    host = argv[1];
  }
  else {
    host = (char*) "localhost";
  }

  const int port = 6665;
  const int device_index = 0;
  try {
    run(host, port, device_index);
    return EXIT_SUCCESS;
  } catch (PlayerError e) {
    cerr << e << endl;
    return EXIT_FAILURE;
  }
}
