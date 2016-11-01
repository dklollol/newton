#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <libplayerc++/playerc++.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "misc.h"
#include "draw.h"
#include "robot.h"
#include "camera.h"
#include "particles.h"
#include "random_numbers.h"
#include "strategy.h"
#include "timing.h"

#include <cstdlib>

using namespace cv;
using namespace PlayerCc;


void set_pull_mode(PlayerClient &robot) {
  robot.SetDataMode(PLAYER_DATAMODE_PULL);
  robot.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1, -1);
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
  vector<particle> particles_resampled(num_particles);

  // Set initial particle values.
  const double offset = 100.0;
  for (int i = 0; i < num_particles; i++) {
    // Random starting points. (x,y) \in [-1000, 1000]^2, theta \in [-pi, pi].
    particles[i].x = world_width * randf() - offset;
    particles[i].y = world_height * randf() - offset;
    particles[i].theta = 2.0 * M_PI * randf() - M_PI;
    particles[i].weight = 1.0 / (double) num_particles;
  }

  // Setup the camera interface.
  camera cam(0, Size(640, 480), false);

  // Initialize player.
  PlayerClient robot(host, port);
  set_pull_mode(robot);
  Position2dProxy pp(&robot, device_index);

  // States
  driving_state_t driving_state;
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
    switch (action_char) {
    case 'q': // Quit
      goto after_loop;
      break;
    }

    // Grab image.
    TIMER_START();
    im = cam.get_colour();
    TIMER_END("Read from camera");

    // Do landmark detection.
    TIMER_START();
    landmark_id = cam.get_object(im, cp, measured_distance, measured_angle);
    TIMER_END("Locate landmark");

    if (landmark_id != object::none) {
      printf("[MEASUREMENTS] Distance: %lf, angle: %lf, rgb: (%lf, %lf, %lf)\n",
             measured_distance, measured_angle, cp.red, cp.green, cp.blue);
    }

    // Correction step: Compute particle weights.
    TIMER_START();
    if (landmark_id == object::none) {
      // No observation; reset weights to uniform distribution.
      for (int i = 0; i < num_particles; i++) {
        particles[i].weight = 1.0 / (double) num_particles;
      }
    }
    else {
      calculate_weights(&particles, measured_distance, measured_angle, landmark_id, cp);
    }
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
    execute_strategy(pp, pos, driving_state, landmark_id,
                     measured_distance, measured_angle);
    printf("[ESTIMATE] Relative translation: x: %lf, y: %lf, turn: %lf\n",
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
    printf("[NOISE ADDITION] dist: %lf, turn: %lf\n", driveVar, turnVar);
    add_uncertainty(particles, driveVar, turnVar);
    TIMER_END("Add uncertainty");

    // Estimate pose.
    TIMER_START();
    est_pose = estimate_pose(particles);
    printf("[ESTIMATE] Current location: x: %f, y: %f, theta: %f\n",
           est_pose.x, est_pose.y, est_pose.theta);
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
  // Stop the robot.
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
