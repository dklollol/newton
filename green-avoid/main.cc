#include <libplayerc++/playerc++.h>     // Robot interaction
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/imgproc/imgproc.hpp>  // OpenCV image processing
#include <ctime>

using namespace PlayerCc;
using namespace cv;

void sleep(double seconds) {
  struct timespec spec;
  spec.tv_sec = (time_t) seconds;
  spec.tv_nsec = (long) ((seconds - (double) spec.tv_sec) * 10e8);
  nanosleep(&spec, NULL);
}

void set_pull_mode(PlayerClient &robot) {
  robot.SetDataMode(PLAYER_DATAMODE_PULL);
  robot.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1, -1);
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

      h = ((double) c0) / 255.0 * 360.0;

      // p[c0_i] = c0;
      // p[c1_i] = 0;
      // p[c2_i] = 0;
      
      if (h > 50.0 && h < 100.0) {
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

int run(char* host, int port, int device_index) {
  // Prepare robot
  PlayerClient robot(host, port);
  set_pull_mode(robot);

  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, device_index);


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


  double move_speed = 0.1;
  double turn_speed = 0;

  pp.SetSpeed(move_speed, turn_speed);

  while (true) {
    // Get picture
    cvWaitKey(4);
    cam >> frame;

    if (frame.empty()) {
      fprintf(stderr, "warning: could not get picture\n");
      break;
    }

    do_work(frame);

    // Show frame
    //cvtColor(frame, frame, CV_HSV2BGR, 0);    
    imshow(WIN_RF, frame);

    //sleep(0.1);
  }

  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
  char* host;
  if (argc > 1) {
    host = argv[1];
  }
  else {
    host = (char*) "localhost";
  }

  const int port = 6665;
  const int device_index = 0;
  try {
    return run(host, port, device_index);
  } catch (PlayerCc::PlayerError e) {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }
}
