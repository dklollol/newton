#include "cam.h"
#include <stdio.h>

int main() {
  // Get an OpenCV camera handle.
  VideoCapture cam(-1);

  // Maybe comment this out to get a larger resolution from your webcam.
  // cam.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
  // cam.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

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

  Box box0;
  Box box1;

  while ((char) cvWaitKey(4) != 32) {
    cam >> frame;
    box0.found = false;
    box0 = do_work(frame);
    imshow(WIN_RF, frame);
  }

  while ((char) cvWaitKey(4) != 32) {
    cam >> frame;
    box1.found = false;
    box1 = do_work(frame);
    imshow(WIN_RF, frame);
  }

  // Print debug information.
  printf("box0 height: %d\n", box0.height);
  printf("box1 height: %d\n", box1.height);

  // Calculate distance with prior knowledge.
  double fov = 106.0; // Niels' camera, fix this
  double box_height = 21.0; // Fix this
  printf("distance with prior knowledge: %lf\n",
         distance_height_known((double) box1.height, get_cam_height(cam),
                               box_height, fov));

  // Calculate distance with two pictures.
  double dist = 25.0;
  printf("distance with two pictures: %lf\n",
         distance_two_pictures(dist,
                               (double) box0.height, (double) box1.height));

  return EXIT_SUCCESS;
}
