#include "cam.h"
#include <stdio.h>
int main() {
  // Get an OpenCV camera handle
  VideoCapture cam(-1);

  cam.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
  cam.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
  
  if (!cam.isOpened()) {
    fprintf(stderr, "error: could not open camera\n");
    return EXIT_FAILURE;
  }

  // Initialise the GUI
  const char *WIN_RF = "Newton CAM";
  namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
  cvMoveWindow(WIN_RF, 400, 0);
  char key;
  // Prepare frame storage
  Mat frame;

  Box box0;
  box0.found = false;
  Box box1;
  box1.found = false;

  while ((char) cvWaitKey(4) != 32) {
    cam >> frame;
    box0 = do_work(frame);
    imshow(WIN_RF, frame);
  }


  while ((char) cvWaitKey(4) != 32) {
    cam >> frame;
    box1 = do_work(frame);
    imshow(WIN_RF, frame);
  }


  // double fov = 106.0; // Niels' camera, fix this
  // double box_height = 14.5; // Fix this
  // printf("distance: %lf\n",
  //        distance_height_known((double) box.height, get_cam_height(cam),
  //                              box_height, fov));
  double dist = 50.0;
  printf("box0 height: %d\n", box0.height);
  printf("box1 height: %d\n", box1.height);
  printf("distance: %lf\n",
         distance_two_pictures(dist,
                               (double) box0.height, (double) box1.height));

  return EXIT_SUCCESS;
}
