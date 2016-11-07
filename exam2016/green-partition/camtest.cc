#include "cam.h"
#include <stdio.h>

int main() {
  // Get an OpenCV camera handle.
  VideoCapture cam(-1);

  // Maybe comment this out to get a larger resolution from your webcam.
  cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

  if (!cam.isOpened()) {
    fprintf(stderr, "error: could not open camera\n");
    return EXIT_FAILURE;
  }

  // Initialise the GUI.
  const char *WIN_RF = "Newton CAM";
  namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
  cvMoveWindow(WIN_RF, 400, 0);

  // Prepare frame storage.
  Mat frame;
  Mat frame2;

  while (true) {
    cvWaitKey(1);
    cam >> frame;

  frame2 = filter_green(frame);

  std::vector<Box> boxes = process_vertical_lines(frame2);

  size_t n_channels = frame.channels();
  size_t n_rows = frame.rows;
  size_t n_cols = frame.cols * n_channels;

  size_t x, y;

  size_t blue_index, green_index, red_index;
  uint8_t blue, green, red;

  uint8_t* pixels = frame.ptr<uint8_t>(0);

  for (size_t i = 0; i < boxes.size(); i++) {
    for (x = boxes[i].start; x <= boxes[i].end; x += n_channels) {
      for(y = 0; y < n_rows; y++) {
        blue_index = y * n_cols + x;
        pixels[blue_index] = 255;
        pixels[blue_index + 1] = 255;
        pixels[blue_index + 2] = 255;
      }
    }
  }


    imshow(WIN_RF, frame);
  }

  return EXIT_SUCCESS;
}
