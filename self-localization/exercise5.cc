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

#include <cstdlib>

using namespace cv;
using namespace PlayerCc;


#define KEY_UP    65362
#define KEY_DOWN  65364
#define KEY_LEFT  65361
#define KEY_RIGHT 65363

enum state {searching, align, approach,
            drive_around_landmark,
            drive_to_center, arrived_at_center};

void say(string text) {
  std::system((string("espeak '") + string(text) + string("'")).c_str());
}

void set_pull_mode(PlayerClient &robot) {
  robot.SetDataMode(PLAYER_DATAMODE_PULL);
  robot.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1, -1);
}

void run(char* host, int port, int device_index) {
  // Constants
  double world_width = 500.0;
  double world_height = 500.0;
  double stop_dist = 150.0; // The target distance from the first landmark.


  // The GUI
  const char *map = "World map";
  const char *window = "Robot view";
  cv::Mat world(cvSize (world_width, world_height), CV_8UC3);
  cv::namedWindow(map, CV_WINDOW_AUTOSIZE);
  cv::namedWindow(window, CV_WINDOW_AUTOSIZE);
  cv::moveWindow(window, 500.0, 20.0);

  // Initialize particles.
  const int num_particles = 5000;
  std::vector<particle> particles(num_particles);
  std::vector<particle> particles_resampled(num_particles);

  for (int i = 0; i < num_particles; i++) {
    // Random starting points. (x,y) \in [-1000, 1000]^2, theta \in [-pi, pi].
    particles[i].x = world_width * randf() - stop_dist;
    particles[i].y = world_height * randf() - stop_dist;
    particles[i].theta = 2.0 * M_PI * randf() - M_PI;
    particles[i].weight = 1.0 / (double) num_particles;
  }

  // Setup the camera interface.
  camera cam(0, cv::Size(640, 480), false);

  // Initialize player.
  PlayerClient robot(host, port);
  set_pull_mode(robot);
  Position2dProxy pp(&robot, device_index);

  // Initialize robot state.
  state robot_state;
  pos_t pos;
  robot_state = searching;
  object::type first_landmark_found = object::none;
  double drive_around_landmark_remaining_dist;
  int turned_angle;
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

    // Reset iteration-relative values.
    pos.x = 0.0;
    pos.y = 0.0;
    pos.turn = 0.0;

    // Grab image.
    cv::Mat im;
    // Hack: Empty buffer to get the newest image.
    for (size_t i = 0; i < 30; i++) {
      im = cam.get_colour();
    }

    // Do landmark detection.
    double measured_distance;
    double measured_angle;
    colour_prop cp;
    // Set the above values.
    object::type ID = cam.get_object(im, cp, measured_distance, measured_angle);

    printf("Landmark detection: %s\n", ((ID == object::none) ? "none" :
                                        ((ID == object::vertical) ? "vertical"
                                         : "horizontal")));
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
      // An observation; set the weights.
      double weight_sum = 0.0;
      for (int i = 0; i < num_particles; i++) {
        double weight = landmark(particles[i], measured_distance,
                                 measured_angle, ID);
        particles[i].weight = weight;
        weight_sum += weight;
      }

      for (int i = 0; i < num_particles; i++) {
        particles[i].weight /= weight_sum;
      }
    }

    // Resampling step.  FIXME: Optimize.  Fix the code below.
    size_t j = 0;
    while (j < num_particles) {
      size_t i = (size_t) (randf() * (num_particles - 1));
      if (particles[i].weight > randf()) {
        particles_resampled[j] = particles[i];
        j++;
      }
    }
    particles = particles_resampled;

    // std::vector<double> weightSumGraph;
    // weightSumGraph.reserve(num_particles);
    // for(int i = 0; i < num_particles; i++) {
    //   /*if (std::isnan(weightSumGraph.back())) {
    //     std::cerr<<"OOOOHH .back() nana\n";
    //     }
    //     if (std::isnan(particles[i].weight)) {
    //     std::cerr<<"OOOOHH .i.weight nana\n";
    //     } */
            
    //   weightSumGraph.push_back(weightSumGraph.back() + particles[i].weight);
    // }
    // // printf("SUM GRAPH!\n");
    // // for (int i = 0; i < num_particles; i++) {
    // //   printf("i: %d, %f\n", i, weightSumGraph[i]);
    // // }
    // //printf("last ele %f", weightSumGraph[num_particles-1]);
          
    // // update particles!!
    // std::vector<particle> pickedParticles; //(num_particles);
    // pickedParticles.reserve(num_particles);
    // double z;
    // for (int i = 0; i < num_particles; i++) {
    //   z = randf();
    //   for (int t = 0; t < num_particles; t++) {
    //     /*       if (t == num_particles-1) {
    //              pickedParticles.push_back(particles[t]);
    //              break;
    //              }*/
    //     if (z < weightSumGraph[t]) {
    //       continue;
    //     }
    //     pickedParticles.push_back(particles[t]);
    //     break;
    //   }
    // }
    // particles = pickedParticles;
    // weightSumGraph.clear();
    

    // Add uncertainty.
    add_uncertainty(particles, 5.0, degrees_to_radians(5.0));

    // Estimate pose.
    particle est_pose = estimate_pose(particles);
    printf("est_pose values: x:%f, y:%f, theta:%f\n",
           est_pose.x, est_pose.y, est_pose.theta);

    // Draw the object in the image.
    cam.draw_object(im);

    // Draw visualisation.
    draw_world(est_pose, particles, world);
    cv::imshow(map, world);
    cv::imshow(window, im);

    // Move the robot according to its current state.
    switch (robot_state) {
    case searching: {
      puts("searching");
      say("searching");
      // The robot is turning to find the first landmark.

      if (ID != object::none) {
        first_landmark_found = ID;
        robot_state = align;
      }
      else {
        turn(&pp, &pos, degrees_to_radians(5.0));
      }
      break;
    }

    case align: {
      puts("align");
      say("align");
      // The robot is aligning itself to be pointing directly at the first
      // landmark.

      if (ID == object::none) {
        robot_state = searching;
      }
      else if (fabs(measured_angle) < degrees_to_radians(5.0)) {
        puts("Angle is good!");
        robot_state = approach;
      }
      else {
        puts("Turn in aling");
        turn(&pp, &pos, clamp(measured_angle,
                              degrees_to_radians(-5.0),
                              degrees_to_radians(5.0)));
      }
      break;
    }

    case approach: {
      puts("approach");
      say("Exterminate");
      // The robot is approaching the box to within a set distance.

      if (ID == object::none) {
        //robot_state = searching;
        break;
      }
      else {
        if (measured_distance <= stop_dist) {
          stop_dist = measured_distance;
          turn(&pp, &pos, degrees_to_radians(90.0));
          drive_around_landmark_remaining_dist = stop_dist;
          robot_state = drive_around_landmark;
          turned_angle = 90;
        }
        else {
          drive(&pp, &pos,
                clamp(measured_distance - stop_dist, 0.0, 10.0));
          robot_state = align; // Make sure it's still aligned.
        }
      }
      break;
    }

    case drive_around_landmark: {
      puts("drive around landmark");
      say("Exterminate landmark");
      // The robot is driving around the first landmark in an attempt to locate
      // the second landmark.

      const double drive_dist = 25.0;
      if (ID != object::none && ID != first_landmark_found) {
        robot_state = drive_to_center;
      }
      else if (drive_around_landmark_remaining_dist > 0.0) {
        drive(&pp, &pos, drive_dist);
        drive_around_landmark_remaining_dist -= drive_dist;
      }
      else {
        turn(&pp, &pos, degrees_to_radians(15.0));
        turned_angle -= 15;
        if (turned_angle <= 0) {
          turned_angle = 90;
        }
        drive_around_landmark_remaining_dist = stop_dist * 2.0;
      }
      break;
    }

    case drive_to_center: {
      puts("drive to center");
      say("drive to center");
      // The robot is driving towards the center between the two landmarks.

      // FIXME: I'm not sure this works.

      double move_x = est_pose.x - 150.0;
      if (move_x >= 0) {
        turn(&pp, &pos, -est_pose.theta);
        drive(&pp, &pos, move_x);
      }
      else {
        turn(&pp, &pos, -est_pose.theta + degrees_to_radians(180.0));
        drive(&pp, &pos, -move_x);
      }

      double move_y = -est_pose.y;
      if (move_y >= 0) {
        turn(&pp, &pos, degrees_to_radians(90.0));
        drive(&pp, &pos, move_y);
      }
      else {
        turn(&pp, &pos, degrees_to_radians(-90.0));
        drive(&pp, &pos, -move_y);
      }

      robot_state = arrived_at_center;
      break;
    }

    case arrived_at_center: {
      puts("arrived at center");
      say("arrived at center");
      // The robot has arrived at the center between the two landmarks.

      //do_run = false;
      break;
    }
    }

    // Prediction step: Update all particles according to how much we have
    // moved.
    for (int i = 0; i < num_particles; i++) {
      move_particle(particles[i], pos.x, pos.y, pos.turn);
    }
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
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }
}
