#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "mpi.h"
#include "img_mpi.h"
#include "img_processing.h"

#include <typeinfo>

using namespace cv;
using namespace std;

void update_communication_arrays (const int &p, const int &img_row_num, const int &img_col_num, const int &img_ch_num, int *send_counts , int *send_index)
{
    for(int i = 0; i < p; i++) {
        send_counts[i] = ((((i+1)*img_row_num)/p)-((i*img_row_num)/p))*img_col_num*img_ch_num; //The number of elements is assigned rows * image's cols * image's channels
        send_index[i] = (((i*img_row_num)/p))*img_col_num*img_ch_num;
    }
}

Mat distribute_image(const int &id, const int &img_row_num, const int &img_col_num, const int &img_ch_num, const int *send_counts , const int *send_index, const uchar *img_data)
{   
    
    
    
    // update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    int recv_counts=send_counts[id];
    uchar *sub_img_buffer=new uchar[recv_counts]; 
    
    Mat sub_img(recv_counts/(img_col_num*img_ch_num),img_col_num,CV_8UC3); //Construct the sub image with (assigned rows, image's col number, 3 channels)
    
    //Assign the sublist from process 0 to each process
    MPI_Scatterv(img_data, send_counts, send_index, MPI_UNSIGNED_CHAR, sub_img_buffer, recv_counts, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    sub_img.data=sub_img_buffer;
    delete[] sub_img_buffer;
    return sub_img;
}

void update_image_properties(const int &id, const Mat &img, int &img_row_num, int &img_col_num, int &img_ch_num)
{
    if(id==0)
    {
        img_row_num=img.rows;
        img_col_num=img.cols;
        img_ch_num=img.channels();
    }
    MPI_Bcast( &img_row_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &img_col_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &img_ch_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
}

void print_send_buffers(const int &id, const int &p, int *send_counts , int *send_index)
{
    if(id==0)
    {
        cout<<"send_counts= ";
        for(int i=0;i<p;i++)
        {
            cout<<send_counts[i]<<" ";
        }
        cout<<endl;
        cout<<"send_index= ";
        for(int i=0;i<p;i++)
        {
            cout<<send_index[i]<<" ";
        }
        cout<<endl;
    }
}

int read_selection(const int &id)
{
    string input;
    int selection;
    if(id==0)
    {
        cout<<"please select: 1.img_zooming 2.img_rotation 3.img_brightness 4.img_contrast 5.img_blurring 6.img_grayscale 7.exit"<<endl;
        cin>>input;
        
        for(char const &c: input)
        {
            if (isdigit(c) == 0) return 0;
        }
        selection=stoi(input);
    }
    MPI_Bcast( &selection, 1, MPI_INT, 0, MPI_COMM_WORLD);
    return selection;
}

Mat img_grayscale_mpi(const int &p, const int &id, int *send_counts , int *send_index, const Mat &img)
{
    printf("img_grayscale is working\n");

    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel
	int recv_counts; //Store the size of the sub image's data
	Mat sub_img; //Store the distributed sub-image

	update_image_properties(id, img, img_row_num, img_col_num, img_ch_num);
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
	
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, send_counts, send_index, img.data);
    cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<" id "<<id <<endl;

    return img;
}