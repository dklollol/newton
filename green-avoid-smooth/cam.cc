#include "cam.h"
#include <stdio.h>
#include <iostream>



// 1 is most close, 0 is least close.
double closeness_hue(double hue_target, double hue, double s, double v) {
  double temp;

  if (hue < hue_target) {
    temp = hue_target;
    hue_target = hue;
    hue = temp;
  }
  double value_limit = 0.3;
  double saturation_limit = 0.25; // limits used for filter our white
  if (v < value_limit || s < saturation_limit) {
    return 0;
  }
  double linear = 1.0 - min(hue - hue_target, hue_target + 360.0 - hue) / (360.0 / 2.0);
  double magic_exp = 3.3; // Lowers non-close values more than close values.
  return pow(linear, magic_exp);
}
// finds the center of the expected green object and color a red dot.
Box green_center(Mat I) {
  size_t n_channels = I.channels();
  size_t n_rows = I.rows;
  size_t n_cols = I.cols * n_channels;
  size_t index;
  double temp = 0.0;
  uint8_t* p = I.ptr<uint8_t>(0);
  int x0 = -1;
  int x1 = -1;
  int y0 = -1;
  int y1 = -1;
  double green_value;
  for (size_t row = 0; row < n_rows; row++) {
    for (size_t col = 0; col < n_cols; col+=3) {
      index = row * n_cols + col;
      temp += p[index];
    }
    green_value = temp / 255; // normalize green value
    if (green_value > 45) { // checks if theres something green.
      if (y0 == -1) {
        y0 = row; // set start of green object at sighting
      }
      y1 = row; // keep update last sighting of green object
      p[index+2] = 255; // debug coloring
      //printf("temp: %lf\n", ha);      
    }
    temp = 0.0;
  }
  
  for (size_t col = 0; col < n_cols; col+=3) {
    for (size_t row = 0; row < n_rows; row++) {
      index = row * n_cols + col;
      temp += p[index];
    }
    green_value = temp / 255;
    if (green_value > 45) {
      if (x0 == -1) {
        x0 = col;
      }
      x1 = col;
      p[index+2] = 255;
      // printf("temp: %lf\n", ha);      
    }

    temp = 0.0;
  }
  Box return_box;
  // Nothing green found, thus return picture
  if (x0 < 0 || x1 < 0 || y0 < 0 || y1 < 0 ) {
    return_box.x0 = 0;
    return_box.y0 = 0;
    return_box.x1 = 0;
    return_box.y1 = 0;
    return_box.found = false;
    return return_box;
  }
  return_box.x0 = x0;
  return_box.y0 = y0;
  return_box.x1 = x1;
  return_box.y1 = y1;
  return_box.center = Point((x0+(x1-x0)/2)/3, y0+(y1-y0)/2);
  return_box.found = ((x1 - x0)/3 * (y1 - y0)) > 40 * 40;
  // draws a circle in the middle of the green object
  circle(I, return_box.center, 10, Scalar(0,0,255) , CV_FILLED);
  return return_box;
}

Box do_work(Mat &I) {
  // Color input format: BGR
  cvtColor(I, I, CV_BGR2HSV, 0);
  // Current format: HSV

  size_t n_channels = I.channels();
  size_t n_rows = I.rows;
  size_t n_cols = I.cols * n_channels;

#ifdef DEBUG
  printf("n_channels: %zd\n", n_channels);
  printf("n_rows: %zd\n", n_rows);
  printf("n_cols: %zd\n", n_cols);
#endif

  // Coordinates
  size_t x, y;

  // Channel indices and channels
  size_t c0_i, c1_i, c2_i;
  uint8_t c0, c1, c2;

  // HSV
  double h, s, v;

  const double hue_green = 120.0;
  
  uint8_t* p = I.ptr<uint8_t>(0);
  for(y = 0; y < n_rows; y++) {
    for (x = 0; x < n_cols; x += n_channels) {
      c0_i = y * n_cols + x;
      c1_i = c0_i + 1;
      c2_i = c0_i + 2;
      c0 = p[c0_i];
      c1 = p[c1_i];
      c2 = p[c2_i];

#ifdef DEBUG
      printf("(x:%04zd, y:%04zd) = (%03hhu, %03hhu, %03hhu)\n",
             x, y, c0, c1, c2);
#endif
      h = ((double) c0) *2 ; // Degrees 0..360.
      s = ((double) c1) / 255.0; // 0..1
      v = ((double) c2) / 255.0; // 0..1


      // Continuous with closeness.
      uint8_t green_closeness = (uint8_t) (closeness_hue(hue_green, h, s, v) * 255.0);
      p[c0_i] = green_closeness;
      p[c1_i] = green_closeness;
      p[c2_i] = green_closeness;
     }
  }
  erode(I, I, Mat(), Point(-1, -1), 3);
  dilate(I, I, Mat(), Point(-1, -1), 1);
  const char *WIN_RF = "Newton CAM";
  namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
  cvMoveWindow(WIN_RF, 400, 0);
  Box box = green_center(I);
  imshow(WIN_RF, I);
  return box;
}
