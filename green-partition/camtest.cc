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

  while (true) {
    cvWaitKey(1);
    cam >> frame;
    imshow(WIN_RF, filter_green(frame));
  }

  return EXIT_SUCCESS;
}
