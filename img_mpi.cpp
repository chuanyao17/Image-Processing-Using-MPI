#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "mpi.h"
#include "img_mpi.h"
#include "img_processing.h"

#include <typeinfo>

using namespace cv;
using namespace std;

void create_communication_arrays (int &p, int &img_row_num, int &img_col_num, int &img_ch_num, int *send_counts , int *send_index)
{
    for(int i = 0; i < p; i++) {
        send_counts[i] = ((((i+1)*img_row_num)/p)-((i*img_row_num)/p))*img_col_num*img_ch_num; //The number of elements is assigned rows * image's cols * image's channels
        send_index[i] = (((i*img_row_num)/p))*img_col_num*img_ch_num;
    }
}

uchar* distribute_image(int &p, int &id, int &img_row_num, int &img_col_num, int &img_ch_num, int *send_counts , int *send_index, uchar *img_data, int &recv_counts)
{   
    
    
    
    create_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    recv_counts=send_counts[id];
    uchar *sub_img_buffer=new uchar[recv_counts]; 
    
    
    //Assign the sublist from process 0 to each process
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

int read_selection(int &id)
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