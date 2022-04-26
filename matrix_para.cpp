#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "mpi.h"
#include "matrix_para.h"

using namespace cv;
using namespace std;

void create_communication_arrays (int &p, int &img_row_num, int &img_col_num, int &img_ch_num, int *send_counts , int *send_index)
{
    for(int i = 0; i < p; i++) {
        send_counts[i] = ((((i+1)*img_row_num)/p)-((i*img_row_num)/p))*img_col_num*img_ch_num; //The number of elements is assigned rows * image's cols * image's channels
        send_index[i] = (((i*img_row_num)/p))*img_col_num*img_ch_num;
    }
}

uchar* distribute_image(int &p, int &id, int &img_row_num, int &img_col_num, int &img_ch_num, int *send_counts , int *send_index, int &recv_counts, uchar *img_data)
{   
    
    create_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    recv_counts=send_counts[id];
    uchar *sub_img_buffer=new uchar[recv_counts]; 
    
    Mat sub_img((((id+1)*img_row_num)/p)-((id*img_row_num)/p),img_col_num,CV_8UC3); //Construct the sub image with (assigned row, image's col number, 3 channels)
    cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<endl;

    MPI_Scatterv(img_data, send_counts, send_index, MPI_UNSIGNED_CHAR, sub_img_buffer, recv_counts, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    return sub_img_buffer;
}

void update_image_properties(int &id, Mat &img, int &img_row_num, int &img_col_num, int &img_ch_num)
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

void print_send_buffers(int &id, int &p, int *send_counts , int *send_index)
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