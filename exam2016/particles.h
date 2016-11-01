#ifndef EX5_PARTICLES_H
#define EX5_PARTICLES_H

#include <vector>

#include "camera.h"


/**
 * Data structure for storing particle information (state and weight)
 */
class particle {
public:
    // Constructor
    particle() {};
    particle(double _x, double _y, double _theta = 0, double _weight = 0) {
        x = _x; y = _y; theta = _theta; weight = _weight;
    };

    // Copy constructor
    particle(const particle& p) {
        x = p.x;
        y = p.y;
        theta = p.theta;
        weight = p.weight;
    }

    // Assignment operator
    void operator=(const particle& p) {
        x = p.x;
        y = p.y;
        theta = p.theta;
        weight = p.weight;
    }

    // Data
    double x;
    double y;
    double theta;
    double weight;
};


/**
 * Estimate the pose from particles by computing the average position and orientation over all particles.
 * This is not done using the particle weights, but just the sample distribution.
 */
particle estimate_pose(std::vector<particle> &particles);


/**
 * Add random noise to the particles pose (x,y,theta). Noise is drawn from two zero mean Gaussian distributions, one for
 * (x,y) and another for the orientation theta.
 *  \param sigma - standard deviation for Gaussian noise for location (x,y)
 *  \param sigma_theta - standard deviation for Gaussian noise for orientation theta
 */
void add_uncertainty(std::vector<particle> &particles, double sigma, double sigma_theta);


/**
 * Add random noise to the particles pose (x,y,theta). Noise is drawn from a zero mean Gaussian distributions for
 * (x,y) and from a von Mises distribution for the orientation theta.
 *  \param sigma - standard deviation for Gaussian noise for location (x,y)
 *  \param theta_kappa - the kappa parameter for the von Mises noise for orientation theta
 */
void add_uncertainty_von_mises(std::vector<particle> &particles, double sigma, double theta_kappa);

double prob(double arg, double var);

////////////////////////////////////////////////////////////


/**
 * Apply the dynamical model to the particle.
 */
void move_particle(particle &p, double delta_x, double delta_y, double delta_theta);
/**
 * Calculates and normalize weights of particles.
 */
void calculate_weights(std::vector<particle> *particles, double dist, double angle,
                       object::type ID, colour_prop cp);

/**
 * Resample particles 
 */
void resample(std::vector<particle> *particles);
#endif
