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

particle estimate_pose (vector<particle> &particles) {
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

void move_particle(particle &p, double delta_x, double delta_y, double delta_turn) {
  p.x += delta_x;
  p.y += delta_y;
  p.theta += delta_turn;
}

double landmark(particle &p, double dist, double angle, object::type landmark,
                colour_prop cp) {
  double landmark_y;
  double landmark_x;

  decide_landmark(landmark, &landmark_x, &landmark_y);

  double dist_p = distance(landmark_x, p.x, landmark_y, p.y);
  double dist_diff = fabs(dist - dist_p);

  double angle_p = atan2(landmark_y - p.y, landmark_x - p.x) - p.theta;
  double angle_diff = fabs(angle - angle_p);

  double prob_dist = prob(dist_diff, 50.0);
  double prob_angle = prob(angle_diff, 0.1);
  // printf("landmark; dist: %lf, angle: %lf\n", prob_dist, prob_angle);

  double prob_final = prob_dist * prob_angle;

  return prob_final;
}

void calculate_weights(vector<particle> *particles, double dist, double angle,
                       object::type ID, colour_prop cp) {
  // An observation; set the weights.
  int num_particles = particles->size();
  double weight_sum = 0.0;
  for (int i = 0; i < num_particles; i++) {
    double weight = landmark(particles->at(i), dist,
                             angle, ID, cp);
    particles->at(i).weight = weight;
    weight_sum += weight;
  }

  for (int i = 0; i < num_particles; i++) {
    particles->at(i).weight /= weight_sum;
  }
}

void resample(vector<particle> *particles) {
  size_t num_particles = particles->size();
  vector<particle>particles_resampled(num_particles);
  // Resampling step.
  size_t j = 0;
  while (j < num_particles) {
    size_t i = (size_t) (randf() * (num_particles - 1));
    if (particles->at(i).weight > randf()) {
      particles_resampled[j] = particles->at(i);
      j++;
    }
  }
  *particles = particles_resampled;

  //  // FIXME: Optimize.  Fix the code below.
  // vector<double> weightSumGraph;   // calculate weightsumGraph!
  // weightSumGraph.reserve(num_particles);
  // for(int i = 0; i < num_particles; i++) {
  //   weightSumGraph.push_back(weightSumGraph.back() + particles[i].weight);
  // }

  // // pick random particles!!
  // vector<particle> pickedParticles; //(num_particles);
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


}
