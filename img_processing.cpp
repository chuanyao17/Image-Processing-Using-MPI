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
	cv::imwrite("../zoomed.jpg", dst);
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
void img_blurring(Mat &src)
{
    printf("img_blurring is working\n");
	
    int h = src.rows;
	int w = src.cols;
 
	// 代码实现：3x3 均值模糊
	Mat dst = src.clone();
	for (int row = 1; row < h - 1; row++) {
		for (int col = 1; col < w - 1; col++) {
			// 卷积过程中取周围的 3x3 个像素（包括自身）
			Vec3b p1 = src.at<Vec3b>(row - 1, col - 1);
			Vec3b p2 = src.at<Vec3b>(row - 1, col);
			Vec3b p3 = src.at<Vec3b>(row - 1, col + 1);
			Vec3b p4 = src.at<Vec3b>(row, col - 1);
			Vec3b p5 = src.at<Vec3b>(row, col);
			Vec3b p6 = src.at<Vec3b>(row, col + 1);
			Vec3b p7 = src.at<Vec3b>(row + 1, col - 1);
			Vec3b p8 = src.at<Vec3b>(row + 1, col);
			Vec3b p9 = src.at<Vec3b>(row + 1, col + 1);
 
			// 分通道取值相加
			int b = p1[0] + p2[0] + p3[0] + p4[0] + p5[0] + p6[0] + p7[0] + p8[0] + p9[0];
			int g = p1[1] + p2[1] + p3[1] + p4[1] + p5[1] + p6[1] + p7[1] + p8[1] + p9[1];
			int r = p1[2] + p2[2] + p3[2] + p4[2] + p5[2] + p6[2] + p7[2] + p8[2] + p9[2];
 
			// 分通道求均值
			dst.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(b / 9);
			dst.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(g / 9);
			dst.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(r / 9);
		}
	}
	imshow("2--blur", dst);
 
	// 直接调用 OpenCV API: 3x3 均值模糊
	// Mat dst_opencv;
	// blur(src, dst_opencv, Size(3, 3), Point(-1, -1), BORDER_DEFAULT);
	// imshow("3--blur(OpenCV API)", dst_opencv);
 
	waitKey(0);
    destroyAllWindows();
    // waitKey(1);
	return;
}

int img_grayscale()
{
    printf("img_grayscale is working\n");
    return 1;
}