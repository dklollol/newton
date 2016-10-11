#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "camera.h"

using namespace cv;


std::string object::name (object::type t)
{
    static const std::string name_list [] = {
        "None",
        "Horizontal",
        "Vertical"
    };

    return name_list [t];
}

// Constructor / Destructor
camera::camera (const int idx, const cv::Size &imsize, bool useLensUnDistort)
: pattern_size (cvSize (3, 4)), patternUnit(50.0f), imsize(imsize), cam(idx), useLensDistortion(useLensUnDistort)
{

    // Check that camera is opened
    if (!cam.isOpened()) {
        std::cout << "camera::camera: Error - Camera could not be opened and initialized" << std::endl;
        exit(-1);
    }

    grey = cv::Mat(imsize, CV_8UC1);
    colour = cv::Mat(imsize, CV_8UC3);
    mask = cv::Mat(imsize, CV_8UC1);
    //grey = cvCreateImage (imsize, IPL_DEPTH_8U, 1);
    //colour = cvCreateImage (imsize, IPL_DEPTH_8U, 3);
    //mask = cvCreateImage (imsize, IPL_DEPTH_8U, 1);


    // Set camera parameters
    cam.set(CV_CAP_PROP_FRAME_WIDTH, imsize.width);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, imsize.height);
    //cam.set(CV_CAP_PROP_BUFFERSIZE, 1);


    // std::cout << "Camera.width = " << cam.get(CV_CAP_PROP_FRAME_WIDTH)
    //           << "  Camera.height = " << cam.get(CV_CAP_PROP_FRAME_HEIGHT)
    //           << "  Camera.buffersize = " << cam.get(CV_CAP_PROP_BUFFERSIZE) << std::endl;

    // Save results in YAML format
    cv::FileStorage fs("calibration_params.yml", cv::FileStorage::READ );

    if (!fs.isOpened()) {
        std::cout << "camera::camera: Error - Could not open camera YAML calibration file" << std::endl;
        exit(-1);
    }

    fs["camera_matrix"] >> intrinsic_matrix;
    fs["distortion_coefficients"] >> distortion_coeffs;


    std::cout << "camera matrix: " << intrinsic_matrix << std::endl
    << "distortion coeffs: " << distortion_coeffs << std::endl;

    /*
     // Camera parameters from the caibration of the Scorpion camera
     intrinsic_matrix = cvCreateMat(3, 3, CV_64FC1);
     CV_MAT_ELEM(*intrinsic_matrix, double, 0, 1) = 0;
     CV_MAT_ELEM(*intrinsic_matrix, double, 1, 0) = 0;
     CV_MAT_ELEM(*intrinsic_matrix, double, 2, 0) = 0;
     CV_MAT_ELEM(*intrinsic_matrix, double, 2, 1) = 0;
     CV_MAT_ELEM(*intrinsic_matrix, double, 2, 2) = 1;
     CV_MAT_ELEM(*intrinsic_matrix, double, 0, 0) = 476.36883;
     CV_MAT_ELEM(*intrinsic_matrix, double, 0, 2) = 239.23852;
     CV_MAT_ELEM(*intrinsic_matrix, double, 1, 1) = 482.93459;
     CV_MAT_ELEM(*intrinsic_matrix, double, 1, 2) = 245.90425;

     distortion_coeffs = cvCreateMat(1, 4, CV_64FC1);
     CV_MAT_ELEM(*distortion_coeffs, double, 0, 0) = -0.29624;
     CV_MAT_ELEM(*distortion_coeffs, double, 0, 1) =  0.13749;
     CV_MAT_ELEM(*distortion_coeffs, double, 0, 2) = -0.01171;
     CV_MAT_ELEM(*distortion_coeffs, double, 0, 3) =  0.01984;
     */

    // Initialise undistortion map
    //mapx = cvCreateMat (imsize.height, imsize.width, CV_32FC1);
    //mapy = cvCreateMat (imsize.height, imsize.width, CV_32FC1);
    //cvInitUndistortMap (intrinsic_matrix, distortion_coeffs, mapx, mapy);



    if (useLensDistortion) {
        mapx = cv::Mat (imsize, CV_32FC1);
        mapy = cv::Mat (imsize, CV_32FC1);
        cv::initUndistortRectifyMap(intrinsic_matrix, distortion_coeffs, cv::Mat(), intrinsic_matrix, imsize, CV_32FC1, mapx, mapy);
        std::cout << "camera::camera: Using estimate of lens distortion" << std::endl;
    } else
        std::cout << "camera::camera: Ignoring estimate of lens distortion" << std::endl;

    std::cout << "camera::camera: Opening and initializing camera" << std::endl;

}

camera::~camera ()
{
    // Deallocate buffers
    //delete [] corners;
    //cvReleaseImage (&grey);
    //cvReleaseImage (&colour);
    //cvReleaseImage (&mask);

    // Release camera
    //cvReleaseCapture (&cam);

    // Release undistortion map
    //cvReleaseMat (&mapx);
    //cvReleaseMat (&mapy);
}

// Accessor for the OpenCV camera handle
cv::VideoCapture& camera::get_capture ()
{
    return cam;
}

// Image acquisition
cv::Mat camera::get_colour ()
{
    if (useLensDistortion) {
        static cv::Mat distorted;
        cam >> distorted;

        cv::remap (distorted, colour, mapx, mapy, cv::INTER_NEAREST);
    } else
        cam >> colour;

    return colour;
}

cv::Mat camera::get_grey (const cv::Mat& col)
{
    //if (col == NULL)
    //  col = get_colour ();
    cv::cvtColor (col, grey, CV_BGR2GRAY);

    return grey;
}

inline cv::Point2i round_cvPoint (const cv::Point2f &p)
{
    return (cv::Point2i ((int)p.x, (int)p.y));
}

// Object detection
object::type camera::get_object (const cv::Mat &im, colour_prop &p, double &distance, double &angle)
{
    object::type retval = object::none;

    // Detect corners
    get_corners (im, found, corner_count);

    if (found)
    {
        // Determine if the object is horizontal or vertical
        const int delta_x = abs (corners [0].x - corners [2].x);
        const int delta_y = abs (corners [0].y - corners [2].y);
        const bool horizontal = (delta_y > delta_x);
        if (horizontal)
            retval = object::horizontal;
        else
            retval = object::vertical;

        // Compute distances and angles
        double height, patternHeight;
        if (horizontal) {
            height = ((fabs (corners [0].y - corners [2].y) +
                       fabs (corners [9].y - corners [11].y)) / 2.0);
            patternHeight = (pattern_size.width-1.0f) * patternUnit;
        } else {
            height = (fabs (corners [0].y - corners [9].y) +
                      fabs (corners [2].y - corners [11].y)) / 2.0;
            patternHeight = (pattern_size.height-1.0f) * patternUnit;
        }

        //distance = CV_MAT_ELEM(*intrinsic_matrix, double, 1, 1) * patternHeight / (height*10.0f); // in cm
        distance = intrinsic_matrix.at<double>(1, 1) * patternHeight / (height*10.0f); // in cm

        const double center = (corners [0].x + corners [2].x +
                               corners [9].x + corners [11].x) / 4.0;

        angle = -atan2(center - imsize.width / 2.0f, intrinsic_matrix.at<double>(0, 0));


        // Classify object by colour
        //if (im->nChannels == 3)
        if (im.channels() == 3)
        {
            // Extract rectangle corners
            cv::Point2i points [] = {
                round_cvPoint (corners [0]),
                round_cvPoint (corners [2]),
                round_cvPoint (corners [9]),
                round_cvPoint (corners [11])
            };

            // Compute region of interest
            //cvZero (mask);
            //cvFillConvexPoly (mask, points, 4, CV_RGB (255, 255, 255));
            mask = cv::Mat::zeros(imsize, CV_8UC1);
            cv::fillConvexPoly(mask, points, 4, CV_RGB (255, 255, 255));

            // Compute mean colour inside region of interest
            //CvScalar mean_colour = cvAvg (im, mask);
            cv::Scalar mean_colour = cv::mean (im, mask);
            const double red   = mean_colour.val [2];
            const double green = mean_colour.val [1];
            const double blue  = mean_colour.val [0];
            const double sum = red + green + blue;

            p.red = red / sum;
            p.green = green / sum;
            p.blue = blue / sum;
        }
        else
            retval = object::none;
    }

    return retval;
}

void camera::draw_object (cv::Mat& im)
{
    cv::drawChessboardCorners (im, pattern_size, corners, found);
}

// Low-level object detection
std::vector<cv::Point2f>& camera::get_corners (const cv::Mat &im, bool &found, int &corner_count)
{
    if (im.channels() == 3)
    {
        cv::cvtColor (im, grey, CV_BGR2GRAY);
    } else {
        grey = im;
    }

    found = cv::findChessboardCorners (grey, pattern_size, corners,
                                     CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_ADAPTIVE_THRESH /*| CV_CALIB_CB_FAST_CHECK*/);
    if (found)
        cv::cornerSubPix (grey, corners, cvSize (5, 5), cvSize (-1, -1),
                            cvTermCriteria (CV_TERMCRIT_ITER, 3, 0.0));

    return corners;
}
