#include <ctime>
#include "cam.h"
#include <math.h>
#include "calibration.h"
 
/*
  FILE* f = fopen("measurements.txt", "w");
  double dist = 25.0;

  Box distances[8][10]; //250,225,200..75
  // gather samples
  for (int t = 0; t < 8; t++) {
  while ((char) cvWaitKey(4) != 32) {
  cam >> frame;
  box0 = do_work(frame);
  imshow(WIN_RF, frame);
  }
  for (int i = 0; i < 10; i++) {
  cam >> frame;
  box0 = do_work(frame);
  distances[t][i] = box0;
  }
  }
  // calculate distances 225,200..75
  for (int t = 1; t < 8; t++) {
  fprintf(f, "%d centimeters\n", 250-t*25);
  for (int i = 0; i < 10; i++) {
  box0 = distances[t-1][i];
  box1 = distances[t][i];
  fprintf(f, "%lf ",
  distance_two_pictures(dist,
  (double) box0.height, (double) box1.height));
  }
  fprintf(f, "\n");
  }
  
*/

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

  if (!cam.isOpened()) {
    fprintf(stderr, "error: could not open camera\n");
    return EXIT_FAILURE;
  }

  // Prepare frame storage.
  Mat frame;



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
