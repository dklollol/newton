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
  Box box;
  box.found = false;
  char key;
  bool show_raw = false;
  while (true) {
    key = (char) cvWaitKey(4);
    if (key == ' ') {
      show_raw = !show_raw;
    }
    else if (key == 27) { // Esc
      break;
    }

    // Get picture
    cam >> frame;

    if (frame.empty()) {
      fprintf(stderr, "warning: could not get picture\n");
      break;
    }

    if (!show_raw) {
      box = do_work(frame);
    }
    
    // Show frame
    imshow(WIN_RF, frame);

    // Calculate!
    printf("height: %d\n", box.height);
    
    double fov = 106.0; // Niels' camera, fix this
    double box_height = 14.5; // Fix this
    printf("distance: %lf\n",
           distance_height_known((double) box.height, get_cam_height(cam),
                                 box_height, fov));
  }

  return EXIT_SUCCESS;
}
