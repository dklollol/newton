#ifndef EX5_CAMERA_H
#define EX5_CAMERA_H

#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

namespace object
{
  enum type
  {
    none = 0,
    horizontal,
    vertical,
    Landmark_1,
    Landmark_2,
    Landmark_3,
    Landmark_4
  };

  string name (type t);
}

typedef struct _cprop
{
  double red, green, blue;
} colour_prop;

class camera
{
  public:
    // Constructor / Destructor
    camera (const int idx = -1, const Size &imsize = Size(640,480), bool useLensUnDistort = true);
    ~camera ();

    // Accessor for the OpenCV camera handle
    VideoCapture& get_capture ();

    // Image acquisition
    Mat get_colour ();
    Mat get_grey (const Mat& colour);

    // Object detection
    object::type get_object (const Mat &im, colour_prop &p, double &distance, double &angle);
    void draw_object (Mat &im);

    // Low-level object detection
    vector<Point2f>& get_corners (const Mat &im, bool &found, int &corner_count);

  private:
    // Object parameters
    const Size pattern_size; // Size of the checkerboard pattern grid
    const double patternUnit; // Size of one checker square in mm
    const Size imsize; // Size of the camera image

    // Camera handle
    VideoCapture cam;

    // Camera calibration parameters
    Mat intrinsic_matrix, distortion_coeffs, mapx, mapy;

    bool useLensDistortion;

    // Buffers
    Mat grey, colour, mask;
    vector<Point2f> corners;

    // States for object detection
    bool found;
    int corner_count;

};

#endif
