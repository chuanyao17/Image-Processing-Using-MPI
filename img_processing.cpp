#include "img_processing.h"
#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

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
void img_brightness(Mat &src, float alpha, float beta)
{
    printf("img_brightness is working\n");
	Mat dst = src.clone();
	for(int row =0 ; row < src.rows ; row++){
        for(int col = 0 ; col < src.cols ; col++){
            //如果输入图像为三通道 RGB
            if(src.channels() == 3){
                //对每个通道上的数据做计算
                dst.at<Vec3b>(row,col)[0] = saturate_cast<uchar>(src.at<Vec3b>(row,col)[0]*alpha + beta);
                dst.at<Vec3b>(row,col)[1] = saturate_cast<uchar>(src.at<Vec3b>(row,col)[1]*alpha + beta);
                dst.at<Vec3b>(row,col)[2] = saturate_cast<uchar>(src.at<Vec3b>(row,col)[2]*alpha + beta);
                //如果输入图像为单通道 灰度
            }else if(src.channels() == 1){
                //对单一通道做计算
                dst.at<uchar>(row,col) = saturate_cast<uchar>(src.at<uchar>(row,col)*alpha + beta);
            }
        }
        
    }
    imshow("brightness",dst);
    waitKey(0);
	destroyAllWindows();
    return;
}
void img_contrast(Mat &src, float percent)
{
    printf("img_contrast is working\n");
	float alpha = percent / 100.f;
	alpha = max(-1.f, min(1.f, alpha));
	Mat temp = src.clone();
	int row = src.rows;
	int col = src.cols;
	int thresh = 127;
	for (int i = 0; i < row; ++i)
	{
		uchar *t = temp.ptr<uchar>(i);
		uchar *s = src.ptr<uchar>(i);
		for (int j = 0; j < col; ++j)
		{
			uchar b = s[3 * j];
			uchar g = s[3 * j + 1];
			uchar r = s[3 * j + 2];
			int newb, newg, newr;
			if (alpha == 1)
			{
				t[3 * j + 2] = r > thresh ? 255 : 0;
				t[3 * j + 1] = g > thresh ? 255 : 0;
				t[3 * j] = b > thresh ? 255 : 0;
				continue;
			}
			else if (alpha >= 0)
			{
				newr = static_cast<int>(thresh + (r - thresh) / (1 - alpha));
				newg = static_cast<int>(thresh + (g - thresh) / (1 - alpha));
				newb = static_cast<int>(thresh + (b - thresh) / (1 - alpha));
			}
			else {
				newr = static_cast<int>(thresh + (r - thresh) * (1 + alpha));
				newg = static_cast<int>(thresh + (g - thresh) * (1 + alpha));
				newb = static_cast<int>(thresh + (b - thresh) * (1 + alpha));
 
			}
			newr = max(0, min(255, newr));
			newg = max(0, min(255, newg));
			newb = max(0, min(255, newb));
			t[3 * j + 2] = static_cast<uchar>(newr);
			t[3 * j + 1] = static_cast<uchar>(newg);
			t[3 * j] = static_cast<uchar>(newb);
		}
	}
	// return temp;
	imshow("contrast", temp);
	waitKey(0);
    destroyAllWindows();
	return;
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

void img_grayscale(Mat &src)
{
    printf("img_grayscale is working\n");
    Mat temp = src.clone();
	int row = src.rows;
	int col = src.cols;
	for (int i = 0; i < row; ++i)
	{
		uchar *t = temp.ptr<uchar>(i);
		uchar *s = src.ptr<uchar>(i);
		for (int j = 0; j < col; ++j)
		{
			uchar b = s[3 * j];
			uchar g = s[3 * j + 1];
			uchar r = s[3 * j + 2];
			int gray_pixel;

			gray_pixel= r*0.299 + g*0.587 +b*0.114;
			
			gray_pixel = max(0, min(255, gray_pixel));
			t[3 * j + 2] = static_cast<uchar>(gray_pixel);
			t[3 * j + 1] = static_cast<uchar>(gray_pixel);
			t[3 * j] = static_cast<uchar>(gray_pixel);
		}
	}
	// return temp;
	imshow("grayscale", temp);
	waitKey(0);
    destroyAllWindows();
	return;
}