#ifndef GREEN_AVOID_CAM_H
#define GREEN_AVOID_CAM_H

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/imgproc/imgproc.hpp>  // OpenCV image processing
#include <opencv2/opencv.hpp>           // noise filtering
#include <cmath>
using namespace cv;

struct Box {
  size_t start;
  size_t end;
};

Mat filter_green(Mat &image);

std::vector<Box> process_vertical_lines(Mat &image);

#endif
