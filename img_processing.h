#ifndef __IMG_PROCESSING_H__
#define __IMG_PROCESSING_H__

#include "matrix_para.h"
#include "opencv2/opencv.hpp"

using namespace cv;

void img_zooming(Mat &, double, double);
int img_rotation();
int img_brightness();
int img_contrast();
int img_blurring();
int img_grayscale();

#endif