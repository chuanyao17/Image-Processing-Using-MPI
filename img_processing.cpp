#include "img_processing.h"
#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"

using namespace cv;

void img_zooming(Mat &src, double kx, double ky)
{
    printf("img_zooming is working\n");
    int  row = src.rows * kx;
	int  col = src.cols * ky;

	cv::Mat dst(row, col, src.type());
	
    for (int i = 0; i < row; i++)
	{
		int srx = i / kx;
		for (int j = 0; j < col; j++)
		{
			int sry = j /ky;
			dst.at<cv::Vec3b>(i, j) = src.at<cv::Vec3b>(srx, sry);
		}
	}
	cv::imwrite("../4.jpg", dst);
    // cv::imshow( "image", dst );
    // cv::waitKey(0);
    return; 
}
int img_rotation()
{
    printf("img_rotation is working\n");
    return 1;
}
int img_brightness()
{
    printf("img_brightness is working\n");
    return 1;
}
int img_contrast()
{
    printf("img_contrast is working\n");
    return 1;
}
int img_blurring()
{
    printf("img_blurring is working\n");
    return 1;
}
int img_grayscale()
{
    printf("img_grayscale is working\n");
    return 1;
}