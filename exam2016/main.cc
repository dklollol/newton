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


#define KEY_UP    65362
#define KEY_DOWN  65364
#define KEY_LEFT  65361
#define KEY_RIGHT 65363

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
  Mat world(cvSize (world_width, world_height), CV_8UC3);
  namedWindow(map, CV_WINDOW_AUTOSIZE);
  namedWindow(window, CV_WINDOW_AUTOSIZE);
  moveWindow(window, 500.0, 20.0);

  // Initialize particles.
  const int num_particles = 2000;
  vector<particle> particles(num_particles);
  vector<particle> particles_resampled(num_particles);

  for (int i = 0; i < num_particles; i++) {
    // Random starting points. (x,y) \in [-1000, 1000]^2, theta \in [-pi, pi].
    particles[i].x = world_width * randf() - 150;
    particles[i].y = world_height * randf() - 150;
    particles[i].theta = 2.0 * M_PI * randf() - M_PI;
    particles[i].weight = 1.0 / (double) num_particles;
  }

  // Setup the camera interface.
  camera cam(0, Size(640, 480), false);

  // Initialize player.
  PlayerClient robot(host, port);
  set_pull_mode(robot);
  Position2dProxy pp(&robot, device_index);

  // Initialize robot state.
  state robot_state;
  // initialize Delta values position
  pos_t pos;

  // Main loop.
  bool do_run = true;
  while (do_run) {
    // Wait a little.
    int action = cvWaitKey(4);
    char action_char = (char) action;
    switch (action_char) {
    case 'q': // Quit
      do_run = false;
      break;
    }


    // Grab image.
    Mat im;
    // Hack: Empty buffer to get the newest image.
    /*for (size_t i = 0; i < 1; i++) {
      im = cam.get_colour();
    }
    */
    // timing_t timer;
    // timing_start(&timer);
    TIMER_START();
    im = cam.get_colour();
    TIMER_END("Read from camera");
    // timing_end(&timer);
    // printf("[PROFILING] Read from camera: %zd\n", timer.usecs);
    // Do landmark detection.
    double measured_distance;
    double measured_angle;
    colour_prop cp;
    // Set the above values.
    object::type ID;
    TIMER_START();
    ID = cam.get_object(im, cp, measured_distance, measured_angle);
    TIMER_END("Locate object");

    /*
    printf("Landmark detection: %s\n", ((ID == object::none) ? "none" :
                                        ((ID == object::vertical) ? "vertical"
                                         : "horizontal")));
    printf ("Colour probabilities: %.3f %.3f %.3f\n", cp.red, cp.green, cp.blue);
    */    

    if (ID != object::none) {
      printf("Measured distance: %lf\n", measured_distance);
      printf("Measured angle: %lf\n", measured_angle);
    }
    
    // Correction step: Compute particle weights.
    if (ID == object::none) {
      // No observation; reset weights to uniform distribution.
      for (int i = 0; i < num_particles; i++) {
        particles[i].weight = 1.0 / (double) num_particles;
      }
    }
    else {
      calculate_weights(&particles, measured_distance, measured_angle, ID, cp);
    }

    resample(&particles);
    
    // Reset iteration-relative values.
    pos.x = 0.0;
    pos.y = 0.0;
    pos.turn = 0.0;

    execute_strategy(&pp, &pos, &robot_state, ID, measured_angle, measured_distance);

    // Prediction step: Update all particles according to how much we have
    // moved.
    for (int i = 0; i < num_particles; i++) {
      move_particle(particles[i], pos.x, pos.y, pos.turn);
    }
    double driveVar;
    double turnVar;
    // Add uncertainty.
    tie(driveVar, turnVar) = command_variance(&pos);
    printf("støj vi tilføjer: dist:%f, turn:%f \n", driveVar, turnVar);
    add_uncertainty(particles, driveVar, turnVar);
      
    // Estimate pose.
    particle est_pose = estimate_pose(particles);
    
    printf("est_pose values: x:%f, y:%f, theta:%f\n",
           est_pose.x, est_pose.y, est_pose.theta);
    
    
    // Draw the object in the image.
    cam.draw_object(im);

    // Draw visualisation.
    draw_world(est_pose, particles, world);
    imshow(map, world);
    imshow(window, im);    
 }
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
