#include <vector>
#include <cmath>
#include <iostream>
#include <stdio.h>

#include "particles.h"
#include "random_numbers.h"
#include <libplayerc++/playerc++.h>

double prob(double arg, double var) {
  return 1/sqrt(2*M_PI*var)*exp(-(pow(arg, 2))/2*var);
}


particle estimate_pose (std::vector<particle> &particles)
{
  double x_sum = 0, y_sum = 0, cos_sum = 0, sin_sum = 0;
  const int len = particles.size ();
  for (int i = 0; i < len; i++)
    {
      x_sum += particles[i].x;
      y_sum += particles[i].y;
      cos_sum += cos (particles[i].theta);
      sin_sum += sin (particles[i].theta);
    }
  const double flen = (double)len;
  const double x = x_sum/flen;
  const double y = y_sum/flen;
  const double theta = atan2 (sin_sum/flen, cos_sum/flen);
  return particle (x, y, theta);
}

// XXX: You implement this
void move_particle (particle &p, double delta_x, double delta_y, double delta_theta)
{
  p.x += delta_x;
  p.y += delta_y;
  p.theta += delta_theta;
}

double landmark(particle &p, double dist, double angle, int landmark_id) {
  double landmark_y = 0;
  double landmark_x = landmark_id ? 0 : 300;
  double disti = sqrt(pow(landmark_x - p.x, 2)+ pow(landmark_y - p.y, 2));
  double dist_diff = std::abs(dist - disti);
  
  landmark_x -= p.x;
  landmark_y -= p.y;

  double rotated_landmark_x = landmark_x * cos(p.theta) - landmark_y * sin(p.theta);
  double rotated_landmark_y = landmark_x * sin(p.theta) + landmark_y * cos(p.theta);
  
  double angle_diff = atan2(rotated_landmark_y, rotated_landmark_x);
  
  /*double anglei = acos((landmark_x - p.x)/disti) - p.theta;
    double angle_diff = std::abs(angle - anglei); */
  return prob(dist_diff, 10) * prob(angle_diff, DTOR(2));
}      


void add_uncertainty (std::vector<particle> &particles, double sigma, double sigma_theta)
{
    const int len = particles.size();
    for (int i = 0; i < len; i++)
      {
        particles[i].x += randn (0, sigma);
        particles[i].y += randn (0, sigma);
        particles[i].theta = fmod(particles[i].theta + randn (0, sigma_theta), 2.0f * M_PI);
      }
}


void add_uncertainty_von_mises (std::vector<particle> &particles, double sigma, double theta_kappa)
{
    const int len = particles.size();
    for (int i = 0; i < len; i++)
    {
        particles[i].x += randn (0, sigma);
        particles[i].y += randn (0, sigma);
        particles[i].theta = fmod(rand_von_mises (particles[i].theta, theta_kappa), 2.0f * M_PI) - M_PI;
    }
}
