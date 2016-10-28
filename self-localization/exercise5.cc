#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <libplayerc++/playerc++.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "robot.h"
#include "camera.h"
#include "particles.h" 
#include "random_numbers.h"

using namespace cv;
using namespace PlayerCc;

/*
 * Some colors
 */
#define CRED     CV_RGB(255	, 0, 0)
#define CGREEN   CV_RGB(0, 255, 0)
#define CBLUE    CV_RGB(0, 0, 255)
#define CCYAN    CV_RGB(0, 255, 255)
#define CYELLOW  CV_RGB(255, 255, 0)
#define CMAGENTA CV_RGB(255, 0, 255)
#define CWHITE   CV_RGB(255, 255, 255)
#define CBLACK   CV_RGB(0, 0, 0)

/*
 * Keyboard constants
 */
#define KEY_UP    65362
#define KEY_DOWN  65364
#define KEY_LEFT  65361
#define KEY_RIGHT 65363

/*
 * Landmarks.
 * The robot knows the position of 2 landmarks. Their coordinates are in cm.
 */
#define num_landmarks 2
const cv::Point2i landmarks [num_landmarks] = {
  cv::Point2i (0, 0),
  cv::Point2i (300, 0),
};

/*
 * Colour map for drawing particles. This function determines the colour of a
 * particle from its weight.
 */
CvScalar jet (const double x)
{
  const double r = (x >= 3.0/8.0 && x < 5.0/8.0) * (4.0 * x - 3.0/2.0)
    + (x >= 5.0/8.0 && x < 7.0/8.0)
    + (x >= 7.0/8.0) * (-4.0 * x + 9.0/2.0);
  const double g = (x >= 1.0/8.0 && x < 3.0/8.0) * (4.0 * x - 1.0/2.0)
    + (x >= 3.0/8.0 && x < 5.0/8.0)
    + (x >= 5.0/8.0 && x < 7.0/8.0) * (-4.0 * x + 7.0/2.0);
  const double b = (x < 1.0/8.0) * (4.0 * x + 1.0/2.0)
    + (x >= 1.0/8.0 && x < 3.0/8.0)
    + (x >= 3.0/8.0 && x < 5.0/8.0) * (-4.0 * x + 5.0/2.0);

  return CV_RGB (255.0*r, 255.0*g, 255.0*b);
}

/*
 * Visualization.
 * This functions draws robots position in the world.
 */
void draw_world (particle &est_pose, std::vector<particle> &particles, cv::Mat &im)
{
  const int offset = 100;

  // White background
  im = CWHITE; // assign the colour white to all pixels

  // Find largest weight
  const int len = particles.size ();
  double max_weight = particles [0].weight;
  for (int i = 1; i < len; i++)
    max_weight = std::max (max_weight, particles [i].weight);

  // Draw particles
  for (int i = 0; i < len; i++)
    {
      const int x = (int)particles[i].x + offset;
      const int y = (int)particles[i].y + offset;
      const cv::Scalar colour = jet (particles[i].weight / max_weight);
      cv::circle (im, cv::Point2i (x,y), 2, colour, 2);
      const CvPoint b = cv::Point2i ((int)(particles[i].x + 15.0*cos(particles[i].theta))+offset,
                                     (int)(particles[i].y + 15.0*sin(particles[i].theta))+offset);
      cv::line   (im, cv::Point2i (x,y), b, colour, 2);
    }

  // Draw landmarks
  const cv::Point2i lm0 = cv::Point2i (landmarks[0].x+offset, landmarks[0].y+offset);
  const cv::Point2i lm1 = cv::Point2i (landmarks[1].x+offset, landmarks[1].y+offset);
  cv::circle (im, lm0, 5, CRED, 2);
  cv::circle (im, lm1, 5, CGREEN, 2);

  // Draw estimated robot pose
  const cv::Point2i a = cv::Point2i ((int)est_pose.x+offset, (int)est_pose.y+offset);
  const cv::Point2i b = cv::Point2i ((int)(est_pose.x + 15.0*cos(est_pose.theta))+offset,
                                     (int)(est_pose.y + 15.0*sin(est_pose.theta))+offset);
  cv::circle (im, a, 5, CMAGENTA, 2);
  cv::line   (im, a, b, CMAGENTA, 2);
}

/*************************\
 *      Main program     *
 \*************************/
int main_old(char* host, int port, int device_index)
{ 
  // The GUI
  const char *map = "World map";
  const char *window = "Robot View";
  cv::Mat world(cvSize (500,500), CV_8UC3);
  cv::namedWindow (map, CV_WINDOW_AUTOSIZE);
  cv::namedWindow (window, CV_WINDOW_AUTOSIZE);
  cv::moveWindow (window, 500, 20);

  // Initialize particles
  const int num_particles = 2000;
  std::vector<particle> particles(num_particles);
  for (int i = 0; i < num_particles; i++)
    {
      // Random starting points. (x,y) \in [-1000, 1000]^2, theta \in [-pi, pi].
      particles[i].x = (double)num_particles*randf() - num_particles/2;
      particles[i].y = (double)num_particles*randf() - num_particles/2;
      particles[i].theta = 2.0*M_PI*randf() - M_PI;
      particles[i].weight = 1.0/(double)num_particles;
    }
  particle est_pose = estimate_pose (particles); // The estimate of the robots current pose

  double Wslow = 0.0;
  double Wfast = 0.0;
  double Wavg = 0.0;

  // The camera interface
  camera cam(0, cv::Size(640,480), false);



  // Initialize player (XXX: You do this)
  PlayerClient robot(host, port);
  Position2dProxy pp(&robot, device_index);

  // Driving parameters
  double velocity = 15; // cm/sec
  const double acceleration = 12; // cm/sec^2
  double angular_velocity = 0.0; // radians/sec
  const double angular_acceleration = M_PI/2.0; // radians/sec^2
  pos_t pos;

  enum state {searching, align, approach, right_angle, drive_in_circle};

  state robot_state = searching;

  double turn_diff = 0;
  double distance_diff = 0;

  // Draw map
  draw_world (est_pose, particles, world);
 
  // Main loop
  while (true)
    {
      // // Move the robot according to user input
      int action = cvWaitKey (5);
      switch (action) {
      case KEY_UP:
        velocity += 4.0;
        break;
      case KEY_DOWN:
        velocity -= 4.0;
        break;
      case KEY_LEFT:
        angular_velocity -= 0.2;
        break; 
      case KEY_RIGHT:
        angular_velocity += 0.2;
        break;
      case 'w': // Forward
        velocity += 4.0; 
        break;
      case 'x': // Backwards
        velocity -= 4.0;
        break; 
      case 's': // Stop
        velocity = 0.0;
        angular_velocity = 0.0;
        break;
      case 'a': // Left
        angular_velocity -= 0.2;
        break;
      case 'd': // Right
        angular_velocity += 0.2;
        break;
      case 'q': // Quit
        goto theend;
      }

      //XXX: Make player drive. You do this


      switch (robot_state) {
        case searching:
	  puts("searching\n");
          turn(&pp, &pos, 10.0);
        break;
        case align:
	  puts("align\n");
          turn(&pp, &pos, turn_diff);
        break;
        case approach:
	  puts("approach\n");
          drive(&pp, &pos, distance_diff);
        break;
        case right_angle:
	  puts("right_angle\n");
          turn(&pp, &pos, turn_diff);
	  cvWaitKey(1000);
	  turn(&pp, &pos, 0.0);
	  robot_state = drive_in_circle;
	  break;
        case drive_in_circle:
	  double circumference = 150 * 2 * M_PI;
	  double speed = 20.0;
	  double drive_time = circumference / speed;
	  double turn_rate = 360.0 / drive_time;
	  driveturn(&pp, &pos, speed, turn_rate);
	  cvWaitKey(47000); // fixme: sov kun lidt ad gangen
	  driveturn(&pp, &pos, 0.0, 0.0);
        break;
      }

      // Prediction step
      // Read odometry, see how far we have moved, and update particles.
      // Or use motor controls to update particles (
      //XXX: You do this
      //drive(&pp);

      for (int i = 0; i < num_particles; i++) {
        move_particle(particles[i], pos.speed * cos(particles[i].theta + pos.turn), pos.speed * cos(particles[i].theta + pos.turn), pos.turn);
      }

      add_uncertainty_von_mises(particles, pos.speed, pos.turn);

      pos.speed = 0;
      pos.turn = 0;

      // Grab image
      cv::Mat im = cam.get_colour ();
      int landmark_id = 0;
      // Do landmark detection
      double measured_distance, measured_angle;
      colour_prop cp;
      object::type ID = cam.get_object (im, cp, measured_distance, measured_angle);
      if (ID != object::none)
        {
          printf ("Measured distance: %f\n", measured_distance);
          printf ("Measured angle:    %f\n", measured_angle);
          printf ("Colour probabilities: %.3f %.3f %.3f\n", cp.red, cp.green, cp.blue);

          if (robot_state != right_angle && robot_state != drive_in_circle) {
            if (std::abs(measured_angle) > DTOR(1)) {
              turn_diff = RTOD(measured_angle);
              robot_state = align;
            } else {
              if (std::abs(measured_distance - 150) > 5) {
                distance_diff = measured_distance - 150;
                robot_state = approach;
              } else {
                turn_diff = 90;
                robot_state = right_angle;
              }
            }
          }

          if (ID == object::horizontal)
            {
              // observed an Landmark!
              landmark_id = 1;
              printf ("Landmark is horizontal\n");
            }
          else if (ID == object::vertical)
            {
              // observed an Landmark!
              printf ("Landmark is vertical\n");
            }
          else
            {
              printf ("Unknown landmark type!\n");
              continue;
            }

          // Correction step
          // Compute particle weights
          // XXX: You do this
          Wavg = 0;
          double weightSum = 0;
          double weight;
          for (int i = 0; i < num_particles; i++) {
            weight = landmark(particles[i], measured_distance/10, measured_angle,
                              landmark_id);
            particles[i].weight = weight;
            Wavg += Wavg + 1/num_particles*weight;
            weightSum += weight;
          }
          for (int i = 0; i < num_particles; i++) {
            particles[i].weight /= weightSum;
          }


          //Wslow += xxx * (Wavg - Wslow)
          //Wfast += xxx * (Wavg - Wfast)

          // Resampling step 
          // XXX: You do this
          // Vector containing the cummalative sum of particle weights
          std::vector<double> weightSumGraph;
          weightSumGraph.reserve(num_particles); // one extra length for 0.0 index at start

          for(int i = 0; i < num_particles; i++)
            weightSumGraph.push_back(weightSumGraph.back() + particles[i].weight);

          // update particles!!
          std::vector<particle> newParticles;
          newParticles.reserve(num_particles);
          double z;
          int j;
          for (int i = 0; i < num_particles; i++) {
            z = randf();
            if (z < weightSumGraph[i]) {
              j = i;
              while (weightSumGraph[j] == weightSumGraph[j-1]) {
                j -= 1;
              }
              newParticles.push_back(particles[j]);
            }
          }
          particles = newParticles;

          // removes all weights from vector at resets length 0.
          weightSumGraph.clear(); 
          // Draw the object in the image (for visualisation)
          cam.draw_object (im);

        } else { // end: if (found_landmark)

        // No observation - reset weights to uniform distribution
        for (int i = 0; i < num_particles; i++)
          {
            particles[i].weight = 1.0/(double)num_particles;
          }

      }  // end: if (not found_landmark)

      // Estimate pose
      est_pose = estimate_pose (particles);
      printf("est_pose values: x:%f, y:%f, theta:%f\n", est_pose.x, est_pose.y, est_pose.theta);
      // Visualisation
      draw_world (est_pose, particles, world);
      cv::imshow(map, world);
      cv::imshow(window, im);
    } // End: while (true)

 theend:

  // Stop the robot
  // XXX: You do this
  //pp.SetSpeed(0.0, DTOR(0));

  return 0;
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
    main_old(host, port, device_index);
    return EXIT_SUCCESS;
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }
}
