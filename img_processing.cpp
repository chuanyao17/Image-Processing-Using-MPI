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
	if(row==0)
	{
		return output_img;
	}
	
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
	if(img.rows==0)
	{
		return output_img;
	}
	
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
Mat img_contrast_brightness(const Mat &img, const double &alpha, const double &beta)
{
    Mat output_img=Mat(img.size(), img.type()); //Initialize the output image
	if(img.rows==0)
	{
		return output_img;
	}
	int row=img.rows;
	int col=img.cols;
	
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
    Mat output_img=Mat(img.size(), img.type()); //Initialize the output image
	if(img.rows==0)
	{
		return output_img;
	}
	// int gaussian_kernel[9]={1,2,1,2,4,2,1,2,1}; //Need to divided by 16 for all elements when calculating	
	// double gaussian_kernel[25]={1,4,7,4,1,4,16,26,16,4,7,26,41,26,7,
	// 4,16,26,16,4,1,4,7,4,1}; //Need to divided by 273 for all elements when calculating
	// int gaussian_kernel[25]={1,4,7,4,1,4,16,26,16,4,7,26,41,26,7,4,16,26,16,4,1,4,7,4,1}; //Need to divided by 16 for all elements when calculating
	int kernel_size=border*2+1;
	int row=img.rows;
	int col=img.cols;

	output_img=Mat(row-(border*2),col-(border*2), img.type());//Initialize the output image
	// cout<<"out_row= "<<output_img.rows<<" out_col= "<<output_img.cols<<" id= "<<id<<endl;

	int b; //Store the result (blue) of the convolution
	int g; //Store the result (green) of the convolution
	int r; //Store the result (red) of the convolution
	

	for(int i=border;i<row-border;i++)
	// for(int i=border;i<3;i++)
	{
		for(int j=border;j<col-border;j++)
		// for(int j=border;j<3;j++)
		{
			
			b=0;
			g=0;
			r=0;
			
			for(int kernel_i=0;kernel_i<kernel_size;kernel_i++)
			{
				for(int kernel_j=0;kernel_j<kernel_size;kernel_j++)
				{
				
					// printf("gaussian_kernel[%d]=%d\n",(kernel_i*kernel_size+kernel_j),gaussian_kernel[kernel_i*kernel_size+kernel_j]);
					
					if(blur_method==averaging_blur)
					{
						// Average_blurr
						b+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[0]);
						g+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[1]);
						r+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[2]);
						// cout<<"averaging_blur loop"<<endl;
					}
					else if(blur_method==gaussian_blur)
					{
						//Gaussian_blurr
						b+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[0]*gaussian_kernel[kernel_i*kernel_size+kernel_j]);
						g+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[1]*gaussian_kernel[kernel_i*kernel_size+kernel_j]);
						r+=(img.at<Vec3b>((i+kernel_i-border),(j+kernel_j-border))[2]*gaussian_kernel[kernel_i*kernel_size+kernel_j]);
					}

					// printf("(%d,%d), src(%d,%d), output(%d,%d), kernel(%d), id=%d\n",i,j,(i+kernel_i-border),(j+kernel_j-border),(i-border),(j-border),(kernel_i*kernel_size+kernel_j),id);
				}
			}

			// printf("b =%d,%d g=%d,%d r=%d,%d \n",b_n/16,b/16,g_n/16,g/16,r_n/16,r/16);
			if(blur_method==averaging_blur)
			{
				//Average_blurr
				output_img.at<Vec3b>(i-border,j-border)[0]=saturate_cast<uchar>(b / (kernel_size*kernel_size));
				output_img.at<Vec3b>(i-border,j-border)[1]=saturate_cast<uchar>(g / (kernel_size*kernel_size));
				output_img.at<Vec3b>(i-border,j-border)[2]=saturate_cast<uchar>(r / (kernel_size*kernel_size));
				// cout<<"averaging_blur color"<<endl;
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