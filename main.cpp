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
        imshow("Display Image", img);
        waitKey(0);

        img_row_num=img.rows;
        img_col_num=img.cols;
        img_ch_num=img.channels();

    }
    //Send the image properties from the ROOT to other processes:
    MPI_Bcast( &img_row_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &img_col_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &img_ch_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    cout<<"rows= "<<img_row_num<<" cols= "<<img_col_num<<" channels= "<<img_ch_num<<" processor= "<<id<<endl;

    
    
    
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