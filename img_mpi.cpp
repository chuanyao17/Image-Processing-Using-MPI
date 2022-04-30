#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "mpi.h"
#include "img_mpi.h"
#include "img_processing.h"

#include <typeinfo>

using namespace cv;
using namespace std;

void update_communication_arrays(const int &p, const int &img_row_num, const int &img_col_num, const int &img_ch_num, int *send_counts , int *send_index)
{
    for(int i = 0; i < p; i++) {
        send_counts[i] = ((((i+1)*img_row_num)/p)-((i*img_row_num)/p))*img_col_num*img_ch_num; //The number of elements is assigned rows * image's cols * image's channels
        send_index[i] = (((i*img_row_num)/p))*img_col_num*img_ch_num;
    }
}

void update_send_index(const int &p, int *send_counts , int *send_index)
{
    for(int i=1;i<p;i++)
    {
        send_index[i]=send_index[i-1]+send_counts[i-1]; //Update send_index by send_counts
    }
}

Mat distribute_image(const int &id, const int &img_row_num, const int &img_col_num, const int &img_ch_num, const int *send_counts , const int *send_index, const uchar *img_data)
{   
    
    
    
    // update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    int recv_counts=send_counts[id]; //Store the size of the sub image's data
    // uchar *sub_img_buffer=new uchar[recv_counts]; 
    
    Mat sub_img(recv_counts/(img_col_num*img_ch_num),img_col_num,CV_8UC3); //Construct the sub image with (assigned rows, image's col number, 3 channels)
    
    //Assign the sublist from process 0 to each process
    MPI_Scatterv(img_data, send_counts, send_index, MPI_UNSIGNED_CHAR, sub_img.data, recv_counts, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    // sub_img.data=sub_img_buffer;
    // delete[] sub_img_buffer;
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

// int get_valid_input_int(const int &id)
// {
//     int input;
//     if(id==0)
//     {
//         while(true)
//         {
//             //Get input of type T
//             cin >> input;

//         //Check if the failbit has been set, meaning the beginning of the input
//         //was not type T. Also make sure the result is the only thing in the input
//         //stream, otherwise things like 2b would be a valid int
//             if (cin.fail() || cin.get() != '\n')
//             {
//                 //Set the error state flag back to goodbit
//                 cin.clear();

//                 //Clear the input stream using and empty while loop
//                 while (cin.get() != '\n');

//                 cout<<"Invalid input, not an integer"<<endl;
//                 continue;
//             }
//             break;
//         }
//     }
//     //Send the result until receive the validate input 
// 	MPI_Bcast( &input, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     return input;
// }

// double get_valid_input_double(const int &id)
// {
//     // int selection;
//     // if(id==0)
//     // {
//     //     selection=get_valid_input<int>();
//     // }
    
//     // MPI_Bcast( &selection, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     // return selection;
//     double input;
//     if(id==0)
//     {
//         while(true)
//         {
//             //Get input of type T
//             cin >> input;

//         //Check if the failbit has been set, meaning the beginning of the input
//         //was not type T. Also make sure the result is the only thing in the input
//         //stream, otherwise things like 2b would be a valid int
//             if (cin.fail() || cin.get() != '\n')
//             {
//                 //Set the error state flag back to goodbit
//                 cin.clear();

//                 //Clear the input stream using and empty while loop
//                 while (cin.get() != '\n');

//                 cout<<"Invalid input, not a floating number"<<endl;
//                 continue;
//             }
//             break;
//         }
//     }
//     //Send the result until receive the validate input 
// 	MPI_Bcast( &input, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//     return input;
// }

void img_grayscale_mpi(const int &p, const int &id, int *send_counts , int *send_index, Mat &img)
{
    printf("img_grayscale is working\n");

    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel
	Mat sub_img; //Store the distributed sub-image

	update_image_properties(id, img, img_row_num, img_col_num, img_ch_num);
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
	
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, send_counts, send_index, img.data);
    // cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<" id "<<id <<endl;
    // imshow("image", sub_img);
    // waitKey(0);
    // destroyAllWindows();
    sub_img=img_grayscale(sub_img);
    MPI_Gatherv(sub_img.data, send_counts[id], MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    return;
}

void img_zooming_mpi(const int &p, const int &id, int *send_counts , int *send_index, Mat &img)
{
    if(id==0)
    {
        printf("img_zooming is working\n");
    }
    

    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel
	Mat sub_img; //Store the distributed sub-image

    double height_ratio; //Store the input as the zooming ratio of the row axis
    double width_ratio; //Store the input as the zooming ratio of the col axis

    if(id==0)
    {
        cout<<"please input a number as the zooming ratio of the row axis"<<endl;
    }
    height_ratio=get_valid_input<double>(id);
    if(id==0)
    {
        cout<<"please input a number as the zooming ratio of the col axis"<<endl;
    }
    width_ratio=get_valid_input<double>(id);

    // cout<<"height_ratio= "<<height_ratio<<" width_ratio= "<<width_ratio<<" id= "<<id<<endl;

	update_image_properties(id, img, img_row_num, img_col_num, img_ch_num);
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);

	
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, send_counts, send_index, img.data);
    // cout<<"*******"<<"img_row_num= "<<img_row_num<<" img_col_num= "<<img_col_num<<" sub_row_num= "<<sub_img.rows<<" sub_col_num= "<<sub_img.cols<<" total= "<<sub_img.rows*sub_img.cols*sub_img.channels()<<" id= "<<id<<endl;
    
    // cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<" id "<<id <<endl;
    // imshow("image", sub_img);
    // waitKey(0);
    // destroyAllWindows();
    sub_img=img_zooming(sub_img, height_ratio, width_ratio);

    //Update communication arrays by each sub_img
    send_counts[id]=sub_img.rows*sub_img.cols*sub_img.channels();
    MPI_Allgather(&send_counts[id], 1, MPI_INT, send_counts, 1, MPI_INT, MPI_COMM_WORLD);
    update_send_index(p, send_counts , send_index);
    
     // Reset the input image to the right size 
    MPI_Allreduce(&sub_img.rows, &img_row_num, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    img_col_num=img_col_num*width_ratio;
    // cout<<"img_row_num= "<<img_row_num<<" img_col_num= "<<img_col_num<<" sub_row_num= "<<sub_img.rows<<" sub_col_num= "<<sub_img.cols<<" total= "<<sub_img.rows*sub_img.cols*sub_img.channels()<<" id= "<<id<<endl;
    if (id==0)
    {
        img = cv::Mat( img_row_num, img_col_num, img.type());
    }
    
    MPI_Gatherv(sub_img.data, send_counts[id], MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    return;
}