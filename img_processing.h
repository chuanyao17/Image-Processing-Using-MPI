#ifndef __IMG_PROCESSING_H__
#define __IMG_PROCESSING_H__
#include "opencv2/opencv.hpp"

using namespace cv;

Mat img_zooming(const Mat &, const double &, const double &);
Mat img_rotation(const Mat &, const int &, const int &, const int &, const bool);
void img_brightness(Mat &, float, float);
void img_contrast(Mat &, float);
Mat img_blurring(const Mat &, const int &, const int &, const int &);
Mat img_grayscale(const Mat &);
Vec3b bilinear_interpolation(const Mat &, const int &, const int &, const double &, const double &);
void img_saving(const int &, const Mat &);

#endif