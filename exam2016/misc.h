#ifndef EX5_MISC_H
#define EX5_MISC_H

double degrees_to_radians(double degrees);

double radians_to_degrees(double radians);

double clamp(double n, double lower, double upper);

void sleep(double seconds);

double distance(double x1, double x2, double y1, double y2);

double driveVariance(double x, double y);

double turnVariance(double theta);

#endif
