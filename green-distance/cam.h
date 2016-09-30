#ifndef GREEN_AVOID_CAM_H
#define GREEN_AVOID_CAM_H

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/imgproc/imgproc.hpp>  // OpenCV image processing
#include <opencv2/opencv.hpp>           // noise filtering
#include <cmath>

using namespace cv;

struct Box {
  int x0;
  int y0;
  int x1;
  int y1;
  Point center;
  bool found;
  int width;
  int height;
};

Box do_work(Mat &I);

double get_cam_height(VideoCapture &cam);

double distance_height_known(double box_height_px, double cam_height_px,
                             double box_height_known, double fov_degrees);

double distance_two_pictures(double dist,
                             double box0_height_px, double box1_height_px);
#endif
