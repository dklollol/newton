#include <vector>
#include <math.h>
#include <iostream>

#include "particles.h"
#include "random_numbers.h"

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
  std::cerr << "particle.cc: move_particle not implemented. You should do this." << std::endl;
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
