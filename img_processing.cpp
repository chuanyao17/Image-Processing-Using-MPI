#include <iostream>
#include <cstdio>
#include "mpi.h"
#include "opencv2/opencv.hpp"
#include <math.h> 
#include "img_processing.h"
#include "img_mpi.h"

using namespace cv;
using namespace std;

Vec3b bilinear_interpolation(const Mat &img, double &i, double &j)
{
	double a=i-floor(i); //weighted i
	double b=j-floor(j); //weighted j

	return (1-a)*(1-b)*img.at<cv::Vec3b>(floor(i), floor(j))+a*(1-b)*img.at<cv::Vec3b>(floor(i)+1, floor(j))+a*b*img.at<cv::Vec3b>(floor(i)+1, floor(j)+1)+(1-a)*(b)*img.at<cv::Vec3b>(floor(i), floor(j)+1);
}

void img_zooming(const Mat &src, double kx, double ky)
{
    printf("img_zooming is working\n");
    int  row = src.rows * kx;
	int  col = src.cols * ky;

	cv::Mat dst(row, col, src.type());
	
    for (int i = 0; i < row; i++)
	{
		double srx = i / kx;
		for (int j = 0; j < col; j++)
		{
			double sry = j /ky;
			dst.at<cv::Vec3b>(i, j) = bilinear_interpolation(src, srx, sry);
		}
	}
	// cv::imwrite("../zoomed.jpg", dst);
    cv::imshow( "image", dst );
    cv::waitKey(0);
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

void img_grayscale(int &p, int &id, int *send_counts , int *send_index, Mat &img)
{
    // printf("img_grayscale is working\n");
    // // Mat temp = src.clone();
	// // int row = src.rows;
	// // int col = src.cols;
	// // for (int i = 0; i < row; ++i)
	// // {
	// // 	uchar *t = temp.ptr<uchar>(i);
	// // 	uchar *s = src.ptr<uchar>(i);
	// // 	for (int j = 0; j < col; ++j)
	// // 	{
	// // 		uchar b = s[3 * j];
	// // 		uchar g = s[3 * j + 1];
	// // 		uchar r = s[3 * j + 2];
	// // 		int gray_pixel;

	// // 		gray_pixel= r*0.299 + g*0.587 +b*0.114;
			
	// // 		gray_pixel = max(0, min(255, gray_pixel));
	// // 		t[3 * j + 2] = static_cast<uchar>(gray_pixel);
	// // 		t[3 * j + 1] = static_cast<uchar>(gray_pixel);
	// // 		t[3 * j] = static_cast<uchar>(gray_pixel);
	// // 	}
	// // }
	// int img_row_num; //Store the number of the input image's row
    // int img_col_num; //Store the number of the input image's col
    // int img_ch_num; //Store the number of the input image's channel
	// int recv_counts; //Store the size of the sub image's data
	// uchar *sub_img_buffer; //Store the distributed sub-image's data of each process

	// update_image_properties(id, img, img_row_num, img_col_num, img_ch_num);
	
	// sub_img_buffer=distribute_image(p, id, img_row_num, img_col_num, img_ch_num, send_counts, send_index, img.data, recv_counts);

	
	// uchar gray;
	// uchar b;
	// uchar g;
	// uchar r;
	// for ( int i = 0; i < recv_counts; i += img_ch_num )
	// {
	// 	// get the pixel:
	// 	b = sub_img_buffer[i];
	// 	g = sub_img_buffer[i+1];
	// 	r = sub_img_buffer[i+2];

	// 	gray=r*0.299 + g*0.587 +b*0.114;
		
	// 	// for example, swap the blue and the red:
	// 	sub_img_buffer[i]=gray;
	// 	sub_img_buffer[i+1]=gray;
	// 	sub_img_buffer[i+2]=gray;
	// }
	
	// // Mat sub_img((((id+1)*img_row_num)/p)-((id*img_row_num)/p),img_col_num,CV_8UC3); //Construct the sub image with (assigned row, image's col number, 3 channels)
    // // cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<endl;
	// // sub_img.data=sub_img_buffer;
	// // imshow("Display Image", sub_img);
    // // waitKey(0);
	// // string tmp=to_string(id)+"gray_image.jpg";
	// // cout<<"id= "<<id<<" tmp= "<<tmp<<endl;
	// // imwrite( tmp, sub_img);

	// //Send the sub images' buffers back to the process 0, gathering the complete image
	// MPI_Gatherv(sub_img_buffer, recv_counts, MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    // delete[] sub_img_buffer;

	// // imshow("grayscale", temp);
	// // waitKey(0);
    // // destroyAllWindows();
	// return;
}