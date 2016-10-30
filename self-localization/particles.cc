#include <vector>
#include <cmath>
#include <iostream>
#include <stdio.h>

#include "particles.h"
#include "camera.h"
#include "misc.h"
#include "random_numbers.h"


double prob(double arg, double var) {
  return ((1.0 / sqrt(2.0 * M_PI * var))
          * exp(-(pow(arg, 2.0)) / (2.0 * var)));
}

particle estimate_pose (std::vector<particle> &particles) {
  double x_sum = 0, y_sum = 0, cos_sum = 0, sin_sum = 0;
  const int len = particles.size ();
  for (int i = 0; i < len; i++) {
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

void add_uncertainty (std::vector<particle> &particles,
                      double sigma, double sigma_theta) {
  const int len = particles.size();
  for (int i = 0; i < len; i++) {
    particles[i].x += randn (0, sigma);
    particles[i].y += randn (0, sigma);
    particles[i].theta = fmod(particles[i].theta + randn (0, sigma_theta), 2.0f * M_PI);
  }
}

void add_uncertainty_von_mises (std::vector<particle> &particles,
                                double sigma, double theta_kappa) {
  const int len = particles.size();
  for (int i = 0; i < len; i++) {
    particles[i].x += randn (0, sigma);
    particles[i].y += randn (0, sigma);
    particles[i].theta = fmod(rand_von_mises (particles[i].theta, theta_kappa),
                              2.0f * M_PI) - M_PI;
  }
}

////////////////////////////////////////////////////////////

void move_particle(particle &p, double delta_x, double delta_y, double delta_turn) {
  p.x += delta_x;
  p.y += delta_y;
  p.theta += delta_turn;
}

double landmark(particle &p, double dist, double angle, object::type landmark_id) {
  double landmark_y = 0.0;
  double landmark_x = ((landmark_id == object::horizontal) ? 0.0 : 300.0);

  double dist_p = sqrt(pow(landmark_x - p.x, 2.0)
                       + pow(landmark_y - p.y, 2.0));
  double dist_diff = fabs(dist - dist_p);
  double dist_diff_max = sqrt(2.0 * pow(500.0, 2.0)); // Not really, but easy.
  double dist_diff_norm = clamp(dist_diff, 0.0, dist_diff_max) / dist_diff_max;

  double angle_p = atan2(landmark_y - p.y, landmark_x - p.x) - p.theta;
  double angle_diff = fabs(angle - angle_p);
  double angle_diff_norm = angle_diff; // Seems okay.

  double prob_dist = prob(dist_diff_norm, 0.01);
  double prob_angle = prob(angle_diff_norm, 0.01);

  double prob_final = prob_dist * prob_angle;

  return prob_final;
}
