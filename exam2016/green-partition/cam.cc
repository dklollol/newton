#include "cam.h"
#include <stdio.h>
#include <iostream>

// 1 is most close, 0 is least close.
static double closeness_hue(double hue_target, double hue, double s, double v) {
  double temp;

  if (hue < hue_target) {
    temp = hue_target;
    hue_target = hue;
    hue = temp;
  }
  double value_limit = 0.2;
  double saturation_limit = 0.3; // Limits used for filter our white
  if (v < value_limit || s < saturation_limit) {
    return 0;
  }

  double linear = 1.0 - min(hue - hue_target, hue_target + 360.0 - hue) / (360.0 / 2.0);
  if (linear < 0.7) {
    return 0;
  }

  double magic_exp = 4.3; // Lowers non-close values more than close values.
  return pow(linear, magic_exp);
}

std::vector<Box> process_vertical_lines(Mat &image) {
  size_t n_channels = image.channels();
  size_t n_rows = image.rows;
  size_t n_cols = image.cols * n_channels;

  size_t x, y;
  size_t blue_index;

  uint8_t* pixels = image.ptr<uint8_t>(0);

  size_t row_sum;

  for (x = 0; x < n_cols; x += n_channels) {
    row_sum = 0;

    for(y = 0; y < n_rows; y++) {
      blue_index = y * n_cols + x;
      row_sum += pixels[blue_index];
    }

    row_sum = row_sum / n_rows;

    if (row_sum > 5) { //Threshold
      row_sum = 255;
    }

    pixels[0 * n_cols + x] = row_sum;
  }

  bool found = false;
  size_t start = -1;
  size_t end = -1;

  int n_box = 0;

  std::vector<Box> boxes;

  for (x = 0; x < n_cols; x += n_channels) {
    row_sum = pixels[0 * n_cols + x];

    start:

    if (row_sum == 255) {
      if (!found) {
        found = true;
        start = x;
        end = x;
        n_box += 1;
      } else {
        if (x - end <= 50) {// Error margin of 50 pixles
          end = x;
        } else {
          Box temp_box;
          temp_box.start = start;
          temp_box.end = end;
          boxes.push_back(temp_box);

          found = false;
          goto start;
        }
      }
    }
  }

  if (found) {
    Box temp_box;
    temp_box.start = start;
    temp_box.end = end;
    boxes.push_back(temp_box);
  }

  return boxes;
}

Mat filter_green(Mat &image) {

  cv::Mat image_copy = image.clone();

  // Color input format: BGR
  cvtColor(image_copy, image_copy, CV_BGR2HSV, 0);
  // Current format: HSV

  size_t n_channels = image_copy.channels();
  size_t n_rows = image_copy.rows;
  size_t n_cols = image_copy.cols * n_channels;

  // Coordinates
  size_t x, y;

  // Channel indices and channels
  size_t blue_index, green_index, red_index;
  uint8_t blue, green, red;

  // HSV
  double hue, saturation, value;

  const double hue_green = 120.0;

  uint8_t* pixels = image_copy.ptr<uint8_t>(0);

  for(y = 0; y < n_rows; y++) {
    for (x = 0; x < n_cols; x += n_channels) {
      blue_index = y * n_cols + x;
      green_index = blue_index + 1;
      red_index = blue_index + 2;

      blue = pixels[blue_index];
      green = pixels[green_index];
      red = pixels[red_index];

      hue = ((double) blue) * 2; // Degrees 0..360.
      saturation = ((double) green) / 255.0; // 0..1
      value = ((double) red) / 255.0; // 0..1

      // Continuous with closeness.
      uint8_t green_closeness = (uint8_t) (closeness_hue(hue_green, hue, saturation, value) * 255.0);
      pixels[blue_index] = green_closeness;
      pixels[green_index] = green_closeness;
      pixels[red_index] = green_closeness;
    }
  }

  erode(image_copy, image_copy, Mat(), Point(-1, -1), 1);
  dilate(image_copy, image_copy, Mat(), Point(-1, -1), 1);

  /*
  std::vector<Box> boxes = process_vertical_lines(image_copy);

  for (size_t i = 0; i < boxes.size(); i++) {
    for (x = boxes[i].start; x <= boxes[i].end; x += n_channels) {
      for(y = 0; y < n_rows; y++) {
        blue_index = y * n_cols + x;
        pixels[blue_index] = 255;
        pixels[blue_index + 1] = 255;
        pixels[blue_index + 2] = 255;
        printf("Box %lu: %lu to %lu\n", i + 1, boxes[i].start, boxes[i].end);
      }
    }
  }
  */

  return image_copy;
}
