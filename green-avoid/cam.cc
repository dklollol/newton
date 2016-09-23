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
  double saturation_limit = 0.25;
  if (v < value_limit || s < saturation_limit) {
    return 0;
  }
  double linear = 1.0 - min(hue - hue_target, hue_target + 360.0 - hue) / (360.0 / 2.0);
  double magic_exp = 3.3; // Lowers non-close values more than close values.
  return pow(linear, magic_exp);
}

void green_center(Mat I) {
  size_t n_channels = I.channels();
  size_t n_rows = I.rows;
  size_t n_cols = I.cols * n_channels;
  size_t index;
  double sum_cols[n_cols];
  double sum_rows[n_rows];
  double temp = 0.0;
  uint8_t* p = I.ptr<uint8_t>(0);
  int x0 = -1;
  int x1 = -1;
  int y0 = -1;
  int y1 = -1;
  for (size_t row = 0; row < n_rows; row++) {
    for (size_t col = 0; col < n_cols; col+=3) {
      index = row * n_cols + col;
      temp += p[index];
    }
    double ha = temp / 255;
    sum_rows[row] = ha;
    if (ha > 35) {
      if (y0 == -1) {
        y0 = row;
      }
      y1 = row;
      p[index+2] = 255;
      //printf("temp: %lf\n", ha);      
    }
    temp = 0.0;
  }
  
  for (size_t col = 0; col < n_cols; col+=3) {
    for (size_t row = 0; row < n_rows; row++) {
      index = row * n_cols + col;
      temp += p[index];
    }
    double ha = temp / 255;
    sum_cols[col] = ha;
    if (ha > 35) {
      if (x0 == -1) {
        x0 = col;
      }
      x1 = col;
      p[index+2] = 255;
      // printf("temp: %lf\n", ha);      
    }

    temp = 0.0;
  }
  // Nothing green found, thus return
  if (x0 < 0 || x1 < 0 || y0 < 0 || y1 < 0 ) {
    return;
  }
  // Drawing the "expected" green box blue 
  /*for (size_t row = y0; row <= y1; row++) {
    for (size_t col = x0; col <= x1; col+=3) {    
      p[row*n_cols+col] = 255;
    }   
    } */
  circle(I, Point((x0+(x1-x0)/2)/3, y0+(y1-y0)/2),10, Scalar(0,0,255) , CV_FILLED);
}

void do_work(Mat &I) {
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

      // Discrete with threshold.
      // const bool green_detect =
      //   (h > hue_green - 30.0
      //    && h < hue_green + 30.0);

      // if (green_detect) {
      //   p[c0_i] = 255;
      //   p[c1_i] = 255;
      //   p[c2_i] = 255;
      // }
      // else {
      //   p[c0_i] = 0;
      //   p[c1_i] = 0;
      //   p[c2_i] = 0;
      // }

    }
  }
  erode(I, I, Mat(), Point(-1, -1), 3);
  dilate(I, I, Mat(), Point(-1, -1), 1);
  green_center(I);
}
