#include <iostream>
#include <cstdio>
#include "mpi.h"
#include "opencv2/opencv.hpp"
#include <math.h> 
#include "img_processing.h"
#include "img_mpi.h"

using namespace cv;
using namespace std;

Vec3b bilinear_interpolation(const Mat &img, const int &dst_i, const int &dst_j, const double &height_ratio, const double &width_ratio)
{
	double src_i; //Store the coordinate i of the source image 
	double src_j; //Store the coordinate j of the source image 
	double a; //Store the weighted src_i
	double b; //Store the weighted src_j

	
	int i; //Store the coordinate of src_i_floor and make sure it is between 0 to the number of rows -1 
	int j; //Store the coordinate of src_j_floor and make sure it is between 0 to the number of cols -1 
	int i_plus; //Store the coordinate of src_i_floor+1 and make sure it is between 0 to the number of rows -1  
	int j_plus; //Store the coordinate of src_j_floor+1 and make sure it is between 0 to the number of cols -1  
	
	src_i=(dst_i+0.5)/height_ratio-0.5; //Align the geometric center of the src image and dst image and get the src_i backwards
	src_j=(dst_j+0.5)/width_ratio-0.5; //Align the geometric center of the src image and dst image and get the src_j backwards

	int src_i_floor=floor(src_i);
	int src_j_floor=floor(src_j);
	
	a=src_i-src_i_floor; 
	b=src_j-src_j_floor; 

	i=min(max(0,src_i_floor),img.rows-1);
	j=min(max(0,src_j_floor),img.cols-1);
	i_plus=min(max(0,src_i_floor+1),img.rows-1);
	j_plus=min(max(0,src_j_floor+1),img.cols-1);

	// cout<<"( "<<dst_i<<","<<dst_j<<" ) "<<" src_i= "<<src_i<<" src_j= "<<src_j<<" a= "<<a<<" b= "<<b<<"( "<<i<<","<<j<<" ) "<<"( "<<i_plus<<","<<j<<" ) "<<"( "<<i_plus<<","<<j_plus<<" ) "<<"( "<<i<<","<<j_plus<<" ) "<<endl;

	return (1-a)*(1-b)*img.at<Vec3b>(i, j)+a*(1-b)*img.at<Vec3b>(i_plus, j)+a*b*img.at<Vec3b>(i_plus, j_plus)+(1-a)*(b)*img.at<Vec3b>(i, j_plus);
}

Mat img_zooming(const Mat &img, const double &height_ratio, const double &width_ratio)
{
    
    int  row = img.rows * height_ratio;
	int  col = img.cols * width_ratio;

	Mat output_img(row, col, img.type());
	
    for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			output_img.at<Vec3b>(i, j) = bilinear_interpolation(img, i, j, height_ratio, width_ratio);
		}
	}
	// cv::imwrite("../zoomed.jpg", dst);
    // cv::imshow( "image", output_img );
    // cv::waitKey(0);
    return output_img; 
}
Mat img_rotation(const Mat &img, const int &row, const int &col, const int &prev_row, const bool clock_wise)
{
    
	Mat output_img=Mat(row,col,img.type()); //Initialize the output image
	
	int src_i; //Store the coordinate i of the source image 
	int src_j; //Store the coordinate j of the source image 
	int r; // src_i = r * j ; src_j= -r* i
	
	int row_offset;
	int col_offset;
	int col_offset_coef;
	int row_offset_coef;

	
	// cout<<" col= "<<img.cols<<" offset= "<<row_offset<<" id= "<<id<<endl;

	if(clock_wise) 
	{
		r=-1; 
		col_offset_coef=1;
		row_offset_coef=0;
	}
	else
	{
		r=1; 
		col_offset_coef=0;
		row_offset_coef=1;
	}
	row_offset=((img.cols-1)*row_offset_coef-prev_row);
	col_offset=(img.rows-1)*col_offset_coef;
	// cout<<"row= "<<row<<" col= "<<col<<endl;

	for(int i=0;i<row;i++)
	{
		for(int j=0;j<col;j++)
		{
			
			src_i= r*(j-col_offset);
			src_j= -r*(i-row_offset);
			
			// printf("(%d,%d), (%d,%d), (%d,%d)\n",i,j,src_i,src_j, (i-row_offset), j);	
			// printf("(%d,%d), (%d,%d), (%d,%d)\n",i,j,src_i,src_j, i, j-col_offset);	
			output_img.at<Vec3b>(i,j)= img.at<Vec3b>(src_i,src_j);
		}
	}
    return output_img;
} 
void img_brightness(Mat &img, float alpha, float beta)
{
    printf("img_brightness is working\n");
	Mat dst = img.clone();
	for(int row =0 ; row < img.rows ; row++){
        for(int col = 0 ; col < img.cols ; col++){
            //如果输入图像为三通道 RGB
            if(img.channels() == 3){
                //对每个通道上的数据做计算
                dst.at<Vec3b>(row,col)[0] = saturate_cast<uchar>(img.at<Vec3b>(row,col)[0]*alpha + beta);
                dst.at<Vec3b>(row,col)[1] = saturate_cast<uchar>(img.at<Vec3b>(row,col)[1]*alpha + beta);
                dst.at<Vec3b>(row,col)[2] = saturate_cast<uchar>(img.at<Vec3b>(row,col)[2]*alpha + beta);
                //如果输入图像为单通道 灰度
            }else if(img.channels() == 1){
                //对单一通道做计算
                dst.at<uchar>(row,col) = saturate_cast<uchar>(img.at<uchar>(row,col)*alpha + beta);
            }
        }
        
    }
    imshow("brightness",dst);
    waitKey(0);
	destroyAllWindows();
    return;
}
void img_contrast(Mat &img, float percent)
{
    printf("img_contrast is working\n");
	float alpha = percent / 100.f;
	alpha = max(-1.f, min(1.f, alpha));
	Mat temp = img.clone();
	int row = img.rows;
	int col = img.cols;
	int thresh = 127;
	for (int i = 0; i < row; ++i)
	{
		uchar *t = temp.ptr<uchar>(i);
		uchar *s = img.ptr<uchar>(i);
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
void img_blurring(Mat &img)
{
    printf("img_blurring is working\n");
	
    int h = img.rows;
	int w = img.cols;
 
	// 代码实现：3x3 均值模糊
	Mat dst = img.clone();
	for (int row = 1; row < h - 1; row++) {
		for (int col = 1; col < w - 1; col++) {
			// 卷积过程中取周围的 3x3 个像素（包括自身）
			Vec3b p1 = img.at<Vec3b>(row - 1, col - 1);
			Vec3b p2 = img.at<Vec3b>(row - 1, col);
			Vec3b p3 = img.at<Vec3b>(row - 1, col + 1);
			Vec3b p4 = img.at<Vec3b>(row, col - 1);
			Vec3b p5 = img.at<Vec3b>(row, col);
			Vec3b p6 = img.at<Vec3b>(row, col + 1);
			Vec3b p7 = img.at<Vec3b>(row + 1, col - 1);
			Vec3b p8 = img.at<Vec3b>(row + 1, col);
			Vec3b p9 = img.at<Vec3b>(row + 1, col + 1);
 
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
	// blur(img, dst_opencv, Size(3, 3), Point(-1, -1), BORDER_DEFAULT);
	// imshow("3--blur(OpenCV API)", dst_opencv);
 
	waitKey(0);
    destroyAllWindows();
    // waitKey(1);
	return;
}

Mat img_grayscale(const Mat &img)
{
    Mat output_img=Mat(img.size(), img.type()); //Initialize the output image
	int row=img.rows;
	int col=img.cols;
	uchar gray;
	uchar b;
	uchar g;
	uchar r;
	
	for(int i=0;i<row;i++)
	{
		for(int j=0;j<col;j++)
		{
			b=img.at<Vec3b>(i,j)[0];
			g=img.at<Vec3b>(i,j)[1];
			r=img.at<Vec3b>(i,j)[2];
			gray=r*0.299 + g*0.587 +b*0.114;
			
			output_img.at<Vec3b>(i,j)[0] = saturate_cast<uchar>(gray);
            output_img.at<Vec3b>(i,j)[1] = saturate_cast<uchar>(gray);
            output_img.at<Vec3b>(i,j)[2] = saturate_cast<uchar>(gray);
		}
	}


    // Mat temp = img.clone();
	// int row = img.rows;
	// int col = img.cols;
	// for (int i = 0; i < row; ++i)
	// {
	// 	uchar *t = temp.ptr<uchar>(i);
	// 	uchar *s = img.ptr<uchar>(i);
	// 	for (int j = 0; j < col; ++j)
	// 	{
	// 		uchar b = s[3 * j];
	// 		uchar g = s[3 * j + 1];
	// 		uchar r = s[3 * j + 2];
	// 		int gray_pixel;

	// 		gray_pixel= r*0.299 + g*0.587 +b*0.114;
			
	// 		gray_pixel = max(0, min(255, gray_pixel));
	// 		t[3 * j + 2] = static_cast<uchar>(gray_pixel);
	// 		t[3 * j + 1] = static_cast<uchar>(gray_pixel);
	// 		t[3 * j] = static_cast<uchar>(gray_pixel);
	// 	}
	// }
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

	// imshow("grayscale", temp);
	// waitKey(0);
    // destroyAllWindows();
	return output_img;
}

void img_saving(const int &id, const Mat &img)
{
    if(id==0)
    {
        printf("img_saving is working\n");
		imwrite("output_image.jpg", img);
    }
    return;
}