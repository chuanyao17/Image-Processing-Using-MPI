#include "img_processing.h"
#include <iostream>
#include <cstdio>
#include "mpi.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    int selection; //Select which img_processing function
    int stop=true; //Stop the program when stop equals to false
    
    Mat img; //Store the input image
    Mat prev_img; //Store the previous modified image
    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel

    int p, id; //The number of the processes and thier id

    int *send_counts; //Store the size of the assgined sub-image of each process
    int *send_index; //Store the start index of the assgined sub-image of each process
    
    int recv_counts; //Store the size of the sub image's data
    uchar *sub_img_buffer; //Store the distributed sub-image's data of each process

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Barrier(MPI_COMM_WORLD);
    

    //Read the image and its properties in the ROOT process:
    if (id==0)
    {
        //Check the valid input
        if (argc!=2) 
        {
            printf("Usage: %s filename\n", argv[0]);
            return -1;
        }

        //Read the image
        img = cv::imread(argv[1]);
        
        //Check if it's empty
        if (!img.data) 
        {
            printf("No image data \n");
            return -1;
        }

        //Display the window infinitely until any keypress
        // imshow("Display Image", img);
        // waitKey(0);

        img_row_num=img.rows;
        img_col_num=img.cols;
        img_ch_num=img.channels();

    }
    //Send the image properties from the ROOT to other processes:
    MPI_Bcast( &img_row_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &img_col_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &img_ch_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    cout<<"rows= "<<img_row_num<<" cols= "<<img_col_num<<" channels= "<<img_ch_num<<" processor= "<<id<<endl;

    //Initialize the sub-image
    send_counts = new int[p]; 
    send_index = new int[p]; 
    sub_img_buffer=distribute_image(p, id, img_row_num, img_col_num, img_ch_num, send_counts , send_index, recv_counts, img.data);
    // create_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    
    // recv_counts=send_counts[id];
    // sub_img_buffer=new uchar[recv_counts]; 
    // Mat sub_img((((id+1)*img_row_num)/p)-((id*img_row_num)/p),img_col_num,CV_8UC3); //Construct the sub image with (assigned row, image's col number, 3 channels)
    // cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<endl;

    

    
    
    //Assign the sublist from process 0 to each process
    // MPI_Scatterv(img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, sub_img_buffer, recv_counts, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    
    //check *send_counts and * send_index
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

    
    
    // sub_img.data=sub_img_buffer;
	// imshow("Display Image", sub_img);
    // waitKey(0);

    // cv::Mat outImage;
    // if (id==0) 
    // { 
    //     outImage = cv::Mat( img.size(), img.type() );
    // }
    // MPI_Gatherv(sub_img_buffer, recv_counts, MPI_UNSIGNED_CHAR, outImage.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    

    //Send the sub images' buffers back to the process 0, gathering the complete image
    MPI_Gatherv(sub_img_buffer, recv_counts, MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    if (id==0)
    {
        imshow("Display Image", img);
        waitKey(0);
    }
    
    
    // printf("program starts:  \n");
    // while (stop)
    // {
    //     printf("please select: 1.img_zooming 2.img_rotation 3.img_brightness 4.img_contrast 5.img_blurring 6.img_grayscale 7.exit\n");  
    //     scanf("%d",&selection); // need to check input , only accept number!
    //     //considering using enum
    //     switch (selection)
    //     {
    //         case 1:
    //             img_zooming(img,8,8);
    //             break;
    //         case 2:
    //             break;
    //         case 3:
    //             img_brightness(img,1.5,10);
    //             break;
    //         case 4:
    //             img_contrast(img,50.f);
    //             break;
    //         case 5:
    //             img_blurring(img);
    //             break;
    //         case 6:
    //             img_grayscale(img);
    //             break;
    //         case 7:
    //             stop=false;
    //             break;
    //         default:
    //             printf("again \n");
    //             break;
    //     }

    // }
    // printf("program ends!\n");
    MPI_Finalize();
    return 0;

}