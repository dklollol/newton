#include <ctime>
#include "cam.h"
#include <math.h>
#include "calibration.h"
 
void set_pull_mode(PlayerClient &robot) {
  robot.SetDataMode(PLAYER_DATAMODE_PULL);
  robot.SetReplaceRule(true, PLAYER_MSGTYPE_DATA, -1, -1);
}

int run(char* host, int port, int device_index) {
  // Prepare robot.
  PlayerClient robot(host, port);
  set_pull_mode(robot);

  Position2dProxy pp(&robot, device_index);
  IrProxy ir(&robot, device_index);

  // Get an OpenCV camera handle.
  VideoCapture cam(-1);
  //cam.set(CV_CAP_PROP_FRAME_WIDTH, 1);
  //cam.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

  // Initialise the GUI.
  const char *WIN_RF = "Newton CAM";
  namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE);
  cvMoveWindow(WIN_RF, 400, 0);
  
  if (!cam.isOpened()) {
    fprintf(stderr, "error: could not open camera\n");
    return EXIT_FAILURE;
  }
  FILE* f = fopen("measurements.txt", "w");
  FILE* f1 = fopen("measurements1.txt", "w");
  double dist = 25.0;
  double move_speed = 0.1;

  Box distances[8][10]; //250,225,200..75
  // Prepare frame storage.
  Mat frame;
  Box box;
  box.found = false;
  double known_height = 29.7;
  double known_fov_degrees = 39.7;
  bool temp;
  // take pictures from distances
  for (int i = 0; i < 8; i++) {
    temp = center_robot_green_box(cam, &pp, &box);
    while(!(temp)) {
      cam>>frame;
      box = do_work(frame);
      imshow(WIN_RF, frame);
      temp = center_robot_green_box(cam, &pp, &box);
    }
    for (int t = 0; t < 10; t++) {
      sleep(1.0);
      cam >> frame;
      box = do_work(frame);
      distances[i][t] = box;
    }

    drive_dist(&pp, dist, move_speed);
  }
  Box box0;
  Box box1;
  // calculate distances 225,200..75
  for (int i = 1; i < 8; i++) {
    fprintf(f, "%d centimeters\n", 250-i*25);
    for (int t = 0; t < 10; t++) {
      box0 = distances[i-1][t];
      box1 = distances[i][t];
      fprintf(f, " %lf ",
              distance_two_pictures(dist,
                                    (double) box0.height, (double) box1.height));
    }
    fprintf(f, "\n");
  }
  for (int i = 0; i < 8; i++) {
    fprintf(f1, "%d centimeters\n", 250-i*25);
    for (int t = 0; t < 10; t++) {
      box0 = distances[i][t];
      fprintf(f1, " %lf", distance_height_known(box0.height, get_cam_height(cam),
                                                known_height, known_fov_degrees));
    }
    fprintf(f1, "\n");
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
