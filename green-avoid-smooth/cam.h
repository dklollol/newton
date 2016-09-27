#ifndef GREEN_AVOID_CAM_H
#define GREEN_AVOID_CAM_H

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/imgproc/imgproc.hpp>  // OpenCV image processing
#include <opencv2/opencv.hpp>           // noise filtering
#include <cmath>


using namespace cv;


//int center_cam = 960;

struct Box {
  int x0;
  int y0;
  int x1;
  int y1;
  Point center;
  bool found;
};

Box do_work(Mat &I);

#endif
