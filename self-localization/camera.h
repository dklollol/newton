#ifndef EX5_CAMERA_H
#define EX5_CAMERA_H

#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace object
{
  enum type
  {
    none = 0,
    horizontal,
    vertical
  };

  std::string name (type t);
}

typedef struct _cprop
{
  double red, green, blue;
} colour_prop;

class camera
{
  public:
    // Constructor / Destructor
    camera (const int idx = -1, const cv::Size &imsize = cv::Size(640,480), bool useLensUnDistort = true);
    ~camera ();

    // Accessor for the OpenCV camera handle
    cv::VideoCapture& get_capture ();

    // Image acquisition
    cv::Mat get_colour ();
    cv::Mat get_grey (const cv::Mat& colour);

    // Object detection
    object::type get_object (const cv::Mat &im, colour_prop &p, double &distance, double &angle);
    void draw_object (cv::Mat &im);

    // Low-level object detection
    std::vector<cv::Point2f>& get_corners (const cv::Mat &im, bool &found, int &corner_count);

  private:
    // Object parameters
    const cv::Size pattern_size; // Size of the checkerboard pattern grid
    const double patternUnit; // Size of one checker square in mm
    const cv::Size imsize; // Size of the camera image

    // Camera handle
    cv::VideoCapture cam;

    // Camera calibration parameters
    cv::Mat intrinsic_matrix, distortion_coeffs, mapx, mapy;

    bool useLensDistortion;

    // Buffers
    cv::Mat grey, colour, mask;
    std::vector<cv::Point2f> corners;

    // States for object detection
    bool found;
    int corner_count;

};

#endif // CAMERA_H
