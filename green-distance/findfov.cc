#include "cam.h"
#include <stdio.h>

int main() {
  // Get an OpenCV camera handle.
  VideoCapture cam(-1);

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

  Box box;

  while ((char) cvWaitKey(4) != 32) {
    cam >> frame;
    box = do_work(frame);
    imshow(WIN_RF, frame);

    // Print debug information.
    printf("box height: %d\n", box.height);

    // Calculate the FOV.
    double known_height = 21.0;
    double known_dist = 50.0;

    double total_height = (get_cam_height(cam) / box.height) * known_height;
    double a = total_height / 2;
    double b = known_dist;
    double c = sqrt(a * a + b * b);
    double A = asin(a / c);
    double fov = radians_to_degrees(A * 2.0);

    printf("camera fov: %lf\n", fov);
  }

  return EXIT_SUCCESS;
}
