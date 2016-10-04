#include <ctime>
#include <libplayerc++/playerc++.h>     // Robot interaction
#include "cam.h"
#include <math.h>
using namespace PlayerCc;

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

Box get_box(VideoCapture cam) {
  Mat frame;
  // Get picture
  cvWaitKey(4);
  cam >> frame;

  if (frame.empty()) {
    fprintf(stderr, "warning: could not get picture\n");
  }

 return do_work(frame);
}

float ir_correction(float range) {
  return range * 0.93569 - 0.103488;
}

bool check_sensor(int index, float threshold, IrProxy &ir) {  
  return (ir_correction(ir.GetRange(index)) < threshold);
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
  // const char *WIN_RF = "Newton CAM";
  // namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
  // cvMoveWindow(WIN_RF, 400, 0);
  
  //imshow(WIN_RF, frame);

  // Prepare frame storage
  Mat frame;
  Box box = get_box(cam);
  bool greenbox_reached = false;
  double move_speed = 0.1;
  double turn_speed = 0;
  double diff;
  double resolution = 640;

  while (true) {

    if (!box.found) {
      pp.SetSpeed(0, DTOR(10));
    } else {	
		diff = ((resolution / 2) - box.center.x) / (resolution / 2);
		
		pp.SetSpeed(0, DTOR(20 * diff));

        if (fabs(diff) < 0.1) {
          pp.SetSpeed(move_speed, DTOR(turn_speed));
          // imshow(WIN_RF, frame);
        }
	}
    box = get_box(cam);
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
