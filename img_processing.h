/*******************************************************************************
  Title          : img_processing.h
  Author         : Chuanyao Lin,
  Created on     : May 10, 2022
  Description    : Implements functions to perform sequential various image processing
  Purpose        :
  Build with     : cmake , please check README
  License        : Copyright 2022 Chuanyao Lin

      This code constains all the fuctions including the usage of OpenCV.
*******************************************************************************/

#ifndef __IMG_PROCESSING_H__
#define __IMG_PROCESSING_H__
#include "opencv2/opencv.hpp"

using namespace cv;

Vec3b bilinear_interpolation(const Mat &, const int &, const int &, const double &, const double &); //Get the dst image's color from src image by bilinear_interpolation
Mat img_zooming(const Mat &, const double &, const double &);
Mat img_rotation(const Mat &, const int &, const int &, const bool);
Mat img_contrast_brightness(const Mat &, const double &, const double &);
Mat img_blurring(const Mat &, const int &, const int &, const int &, const double *, const int&);
Mat img_grayscale(const Mat &);
void img_saving(const int &, const Mat &);

#endif