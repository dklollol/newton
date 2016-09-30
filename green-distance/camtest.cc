#include "cam.h"
#include <stdio.h>
int main() {
  // Get an OpenCV camera handle
  VideoCapture cam(-1);

  if (!cam.isOpened()) {
    fprintf(stderr, "error: could not open camera\n");
    return EXIT_FAILURE;
  }

  // Initialise the GUI
  const char *WIN_RF = "Newton CAM";
  namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
  cvMoveWindow(WIN_RF, 400, 0);

  // Prepare frame storage
  Mat frame;

  Box box0;
  box0.found = false;
  Box box1;
  box1.found = false;

  cam >> frame;
  box0 = do_work(frame);

  char key = (char) cvWaitKey(4);
  key = key;

  cam >> frame;
  box1 = do_work(frame);

  // double fov = 106.0; // Niels' camera, fix this
  // double box_height = 14.5; // Fix this
  // printf("distance: %lf\n",
  //        distance_height_known((double) box.height, get_cam_height(cam),
  //                              box_height, fov));
  double dist = 100.0;
  printf("distance: %lf\n",
         distance_two_pictures(dist,
                               (double) box0.height, (double) box1.height));

  return EXIT_SUCCESS;
}
