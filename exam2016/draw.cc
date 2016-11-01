#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <libplayerc++/playerc++.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "misc.h"
#include "robot.h"
#include "camera.h"
#include "particles.h"
#include "random_numbers.h"
#include "draw.h"

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
 * Landmarks.
 * The robot knows the position of 2 landmarks. Their coordinates are in cm.
 */
#define num_landmarks 4
const Point2i landmarks [num_landmarks] = {
  Point2i (0, 300),
  Point2i (0, 0),
  Point2i (300, 300),
  Point2i (300, 0)
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
  const cv::Point2i lm2 = cv::Point2i (landmarks[2].x+offset, landmarks[2].y+offset);
  const cv::Point2i lm3 = cv::Point2i (landmarks[3].x+offset, landmarks[3].y+offset);
  cv::circle (im, lm0, 5, CRED, 2);
  cv::circle (im, lm1, 5, CGREEN, 2);
  cv::circle (im, lm2, 5, CBLUE, 2);
  cv::circle (im, lm3, 5, CBLACK, 2);
  

  // Draw estimated robot pose
  const cv::Point2i a = cv::Point2i ((int)est_pose.x+offset, (int)est_pose.y+offset);
  const cv::Point2i b = cv::Point2i ((int)(est_pose.x + 15.0*cos(est_pose.theta))+offset,
                                     (int)(est_pose.y + 15.0*sin(est_pose.theta))+offset);
  cv::circle (im, a, 5, CMAGENTA, 2);
  cv::line   (im, a, b, CMAGENTA, 2);
}
