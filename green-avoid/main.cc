#include <ctime>
#include <libplayerc++/playerc++.h>     // Robot interaction
#include "cam.h"

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

void center_robot(PlayerClient &robot, Box &box,  Position2dProxy pp, VideoCapture cam) {
  while (!(box.center.x > 315 && 325 > box.center.x)) {
    if (box.center.x > 320) {
      pp.SetSpeed(0, DTOR(10));
    } else {
      pp.SetSpeed(0, DTOR(-10));
    }
    box = get_box(cam);
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
  //const char *WIN_RF = "Newton CAM";
  //namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
  //cvMoveWindow(WIN_RF, 400, 0);

  // Prepare frame storage
  Mat frame;
  Box box = get_box(cam);
  bool greenbox_reached = false;
  double move_speed = 0.1;
  double turn_speed = 0;

  while (!box.found) {
    pp.SetSpeed(0, DTOR(10));
    box = get_box(cam);
  }
  printf("y0 : %d, y1: %d\n", box.y0, box.y1);
  printf("FOUDN A BOX\n");
  
  while (!greenbox_reached) {
    //printf("x : %d\n", box.center.x);
    //box = get_box(cam);
    while (!(box.center.x > 315 && 325 > box.center.x)) {
      if (box.center.x > 320) {
        pp.SetSpeed(0, DTOR(-10));
      } else {
        pp.SetSpeed(0, DTOR(10));
      }
      box = get_box(cam);
    }
    //center_robot(robot, box , pp , cam);
        //sleep(0.5);
    box = get_box(cam);
    printf("hej\n");
    pp.SetSpeed(move_speed, DTOR(turn_speed));
    sleep(0.5);
    //greenbox_reached = check_sensor(2, 0.15, ir);
  }
  pp.SetSpeed(0, DTOR(turn_speed));
  printf("x0 : %d, y0: %d, x1: %d, y1: %d\n", box.x0, box.y0, box.x1, box.y1);
 
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
