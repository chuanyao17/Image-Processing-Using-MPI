/*******************************************************************************
  Title          : img_processing.cpp
  Author         : Chuanyao Lin,
  Created on     : May 10, 2022
  Description    : Implements functions to perform sequential various image processing
  Purpose        :
  Build with     : cmake , please check README
  License        : Copyright 2022 Chuanyao Lin

      This code constains all the fuctions including the usage of OpenCV.
*******************************************************************************/

#include <iostream>
#include <cstdio>
#include "mpi.h"
#include "opencv2/opencv.hpp"
#include <math.h> 
#include "img_processing.h"
#include "img_mpi.h"

#define averaging_blur 1
#define gaussian_blur 2

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

	//Avoid the out of bonds coordinates
	i=min(max(0,src_i_floor),img.rows-1);
	j=min(max(0,src_j_floor),img.cols-1);
	i_plus=min(max(0,src_i_floor+1),img.rows-1);
	j_plus=min(max(0,src_j_floor+1),img.cols-1);

	return (1-a)*(1-b)*img.at<Vec3b>(i, j)+a*(1-b)*img.at<Vec3b>(i_plus, j)+a*b*img.at<Vec3b>(i_plus, j_plus)+(1-a)*(b)*img.at<Vec3b>(i, j_plus);
}

Mat img_zooming(const Mat &img, const double &height_ratio, const double &width_ratio)
{
    
    int  row = img.rows * height_ratio; //Store the number of zoomed row
	int  col = img.cols * width_ratio;	//Store the number of zoomed col

	Mat output_img(row, col, img.type()); //Initialize the output image
	if(row==0)
	{
		return output_img;
	}
	
	//Go through every pixel of output image to get the color from the source image by using bilinear_interpolation
    for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			output_img.at<Vec3b>(i, j) = bilinear_interpolation(img, i, j, height_ratio, width_ratio);
		}
	}

    return output_img; 
}

Mat img_rotation(const Mat &img, const int &row, const int &col, const bool clock_wise)
{
    
	Mat output_img=Mat(col,row,img.type()); //Initialize the output image
	if(img.rows==0)
	{
		return output_img;
	}
	
	int src_i; //Store the coordinate i of the source image 
	int src_j; //Store the coordinate j of the source image 
	int r; // src_i = r * j ; src_j= -r* i  (the formula of coordinate transformation)
	
	//Output image's coordinates needs offset to get the right coordinates while doing coordinate transformatio of source image after rotation
	int row_offset;
	int col_offset;
	int col_offset_coef;
	int row_offset_coef;

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
	row_offset=((img.cols-1)*row_offset_coef);
	col_offset=(img.rows-1)*col_offset_coef;

	//Go through every pixel of output image to get the color from the source image by coordinate transformation
	for(int i=0;i<col;i++)
	{
		for(int j=0;j<row;j++)
		{
			src_i= r*(j-col_offset);
			src_j= -r*(i-row_offset);
			output_img.at<Vec3b>(i,j)= img.at<Vec3b>(src_i,src_j);
		}
	}
    return output_img;
} 

Mat img_contrast_brightness(const Mat &img, const double &alpha, const double &beta)
{
    Mat output_img=Mat(img.size(), img.type()); //Initialize the output image
	if(img.rows==0)
	{
		return output_img;
	}
	int row=img.rows;
	int col=img.cols;
	
	//Go through every pixel of output image to get the color from the source image by the formula: dst's color = src's color *alpha  + beta
	//alpha decides the ratio of the contrast
	//beta decides the volume of increasing brightness
	//saturate_cast keeps range of the calculated colors between 0~255
	for(int i=0;i<row;i++)
	{
		for(int j=0;j<col;j++)
		{
			output_img.at<Vec3b>(i,j)[0] = saturate_cast<uchar>(img.at<Vec3b>(i,j)[0]*alpha + beta);
            output_img.at<Vec3b>(i,j)[1] = saturate_cast<uchar>(img.at<Vec3b>(i,j)[1]*alpha + beta);
            output_img.at<Vec3b>(i,j)[2] = saturate_cast<uchar>(img.at<Vec3b>(i,j)[2]*alpha + beta);
		}
	}

    return output_img;
}

Mat img_blurring(const Mat &img, const int &id, const int &p, const int &border, const double *gaussian_kernel ,const int& blur_method)
{
	int kernel_size=border*2+1;
	int row=img.rows;
	int col=img.cols;
	Mat output_img=Mat(row-(border*2),col-(border*2), img.type());//Initialize the output image
	if(img.rows==0)
	{
		return output_img;
	}

	int b; //Store the result (blue) of the convolution
	int g; //Store the result (green) of the convolution
	int r; //Store the result (red) of the convolution
	

	//Go through every pixel of output image to get the color from the source image by doing matrix convolution
	for(int i=border;i<row-border;i++)
	{
		for(int j=border;j<col-border;j++)
		{
			
			b=0;
			g=0;
			r=0;
			
			for(int kernel_i=0;kernel_i<kernel_size;kernel_i++)
			{
				for(int kernel_j=0;kernel_j<kernel_size;kernel_j++)
				{	
					if(blur_method==averaging_blur)
					{
						// Average_blurr
						b+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[0]);
						g+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[1]);
						r+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[2]);
					}
					else if(blur_method==gaussian_blur)
					{
						//Gaussian_blurr
						b+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[0]*gaussian_kernel[kernel_i*kernel_size+kernel_j]);
						g+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[1]*gaussian_kernel[kernel_i*kernel_size+kernel_j]);
						r+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[2]*gaussian_kernel[kernel_i*kernel_size+kernel_j]);
					}
				}
			}
			if(blur_method==averaging_blur)
			{
				//Average_blurr
				output_img.at<Vec3b>(i-border,j-border)[0]=saturate_cast<uchar>(b / (kernel_size*kernel_size));
				output_img.at<Vec3b>(i-border,j-border)[1]=saturate_cast<uchar>(g / (kernel_size*kernel_size));
				output_img.at<Vec3b>(i-border,j-border)[2]=saturate_cast<uchar>(r / (kernel_size*kernel_size));
			}
			else if(blur_method==gaussian_blur)
			{
				//Gaussian_blurr
				output_img.at<Vec3b>(i-border,j-border)[0]=saturate_cast<uchar>(b);
				output_img.at<Vec3b>(i-border,j-border)[1]=saturate_cast<uchar>(g);
				output_img.at<Vec3b>(i-border,j-border)[2]=saturate_cast<uchar>(r);
			}
		}
	}
	return output_img;
}

Mat img_grayscale(const Mat &img)
{	
	Mat output_img=Mat(img.size(), img.type()); //Initialize the output image
	if(img.rows==0)
	{
		return output_img;
	}
	int row=img.rows;
	int col=img.cols;
	uchar gray;
	uchar b;
	uchar g;
	uchar r;
	

	//Go through every pixel of output image to get the color from the source image by the formula: dst's color = src's red*0.299 + green*0.587 + blue*0.114 reference from the Matlab
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