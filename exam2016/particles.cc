#include <vector>
#include <cmath>
#include <iostream>
#include <stdio.h>

#include "particles.h"
#include "camera.h"
#include "misc.h"
#include "random_numbers.h"

const double offset = 100.0;

particle estimate_pose (vector<particle> &particles) {
  double x_sum = 0, y_sum = 0, cos_sum = 0, sin_sum = 0;
  const int len = particles.size();
  for (int i = 0; i < len; i++) {
    x_sum += particles[i].x;
    y_sum += particles[i].y;
    cos_sum += cos (particles[i].theta);
    sin_sum += sin (particles[i].theta);
  }
  const double flen = (double)len;
  const double x = x_sum/flen;
  const double y = y_sum/flen;
  const double theta = atan2(sin_sum/flen, cos_sum/flen);
  return particle(x, y, theta);
}

void add_uncertainty (vector<particle> &particles,
                      double sigma, double sigma_theta) {
  const int len = particles.size();
  for (int i = 0; i < len; i++) {
    particles[i].x += randn (0, sigma);
    particles[i].y += randn (0, sigma);
    particles[i].theta = fmod(particles[i].theta + randn (0, sigma_theta), 2.0f * M_PI);
  }
}

void add_uncertainty_von_mises (vector<particle> &particles,
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

double prob(double arg, double var) {
  return ((1.0 / sqrt(2.0 * M_PI * var))
          * exp(-(pow(arg, 2.0)) / (2.0 * var)));
}

void move_particle(particle &p, double delta_x, double delta_y,
                   double delta_turn) {
  p.x += delta_x;
  p.y += delta_y;
  p.theta += delta_turn;
}

double landmark(particle &p, double dist, double angle,
                object::type landmark) {
  double landmark_y = 0.0;
  double landmark_x = 0.0;

  decide_landmark(landmark, &landmark_x, &landmark_y);

  double dist_p = distance(landmark_x, p.x, landmark_y, p.y);
  double dist_diff = fabs(dist - dist_p);

  double angle_p = atan2(landmark_y - p.y, landmark_x - p.x) - p.theta;
  double angle_diff = fabs(angle - angle_p);

  double prob_dist = prob(dist_diff, 50.0);
  double prob_angle = prob(angle_diff, 0.1);

  double prob_final = prob_dist * prob_angle;
  return prob_final;
}

void calculate_weights(vector<particle> *particles, double dist, double angle,
                       object::type landmark_id) {
  int num_particles = particles->size();
  // no landmark detected, weights uniform calculated!
  if (landmark_id == object::none) {
    for (int i = 0; i < num_particles; i++) {
      particles->at(i).weight = 1.0 / (double) num_particles;
    }
    return;
  }
  double weight_sum = 0.0;
  for (int i = 0; i < num_particles; i++) {
    double weight = landmark(particles->at(i), dist, angle, landmark_id);
    particles->at(i).weight = weight;
    weight_sum += weight;
  }

  for (int i = 0; i < num_particles; i++) {
    particles->at(i).weight /= weight_sum;
  }
}

void resample(vector<particle> *particles, object::type landmark_id) {
  size_t num_particles = particles->size();
  size_t extra_particle = landmark_id != object::none ? num_particles*0.01 : 0;
  vector<particle> particles_resampled(num_particles);
  double r = randf() / num_particles;
  double c = particles->at(0).weight;
  size_t i = 0;
  for (size_t m = 0; m < num_particles-extra_particle; m++) {
    double U = r + m * (1.0 / (double) num_particles);
    while (U > c) {
      i++;
      c += particles->at(i).weight;
    }
    particles_resampled[m] = particles->at(i);
  }
  
  for (size_t i = num_particles-extra_particle; i < num_particles; i++) {
    particle temp; 
    // Random starting points. (x,y) \in [-400, 400]^2, theta \in [-pi, pi].
    temp.x = 500 * randf() - offset;
    temp.y = 500 * randf() - offset;
    temp.theta = 2.0 * M_PI * randf() - M_PI;
    temp.weight = 1.0 / (double) num_particles+extra_particle;
    particles_resampled[i] = temp;
  }
  *particles = particles_resampled;
  
}
