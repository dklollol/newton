#include "cam.h"

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

  // Channels
  size_t c0_i, c1_i, c2_i;
  uint8_t c0, c1, c2;

  double h, s, v;
  
  uint8_t* p = I.ptr<uint8_t>(0);
  for(y = 0; y < n_rows; y++) {
    for (x = 0; x < n_cols; x += 3) {
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

      h = ((double) c0) * 2.0; // Degrees 0..360.
      s = ((double) c1) / 255.0; // 0..1
      v = ((double) c2) / 255.0; // 0..1

      const double hue_green = 120.0;
      const bool green_detect =
        (h > hue_green - 30.0
         && h < hue_green + 30.0);

      if (green_detect) {
        p[c0_i] = 255;
        p[c1_i] = 255;
        p[c2_i] = 255;
      }
      else {
        p[c0_i] = 0;
        p[c1_i] = 0;
        p[c2_i] = 0;
      }
    }
  }
}
