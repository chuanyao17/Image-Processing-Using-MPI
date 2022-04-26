#ifndef __IMG_PROCESSING_H__
#define __IMG_PROCESSING_H__

#include "matrix_para.h"
#include "opencv2/opencv.hpp"

using namespace cv;

void img_zooming(Mat &, double, double);
int img_rotation();
void img_brightness(Mat &, float, float);
void img_contrast(Mat &, float);
void img_blurring(Mat &);
void img_grayscale(int &, int &, int *, int *, Mat &);

#endif