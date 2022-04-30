#include <iostream>
#include <cstdio>
#include "mpi.h"
#include "opencv2/opencv.hpp"
#include "img_processing.h"
#include "img_mpi.h"


using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    int selection; //Select which img_processing function
    int stop=true; //Stop the program when stop equals to false
    
    Mat img; //Store the input image
    Mat prev_img; //Store the previous modified image
 

    int p, id; //The number of the processes and thier id

    int *send_counts; //Store the size of the assgined sub-image of each process
    int *send_index; //Store the start index of the assgined sub-image of each process
    
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    
    

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
        cout<<"program starts:  "<<endl;

        

        //Display the window infinitely until any keypress
        imshow("image", img);
        waitKey(0);
        destroyAllWindows();
    }

    //Initialize the sending buffers as communication_arrays for MPI
    send_counts = new int[p]; 
    send_index = new int[p]; 
    
    
    // img_grayscale(p, id, send_counts , send_index, img);
    
    // img_zooming(img,0.5,2);

    MPI_Barrier(MPI_COMM_WORLD);
    while (stop)
    {
        if(id==0)
        {
            cout<<"please select: 1.img_zooming 2.img_rotation 3.img_brightness 4.img_contrast 5.img_blurring 6.img_grayscale 7.exit"<<endl;
        }
        selection=get_valid_input<int>(id);
        // char test=get_valid_input<char>(id);
        // cout<<test<<" id= "<< id<<endl;
        // cout<<selection<<" id= "<< id<<endl;
        switch (selection)
        {
            case 1:
                prev_img=img; 
                img_zooming_mpi(p, id, send_counts , send_index, img);
                break;
            case 2:
                break;
            case 3:
                // img_brightness(img,1.5,10);
                break;
            case 4:
                // img_contrast(img,50.f);
                break;
            case 5:
                // img_blurring(img);
                break;
            case 6:
                //Initialize the previous image to be the input image
                prev_img=img; 
                img_grayscale_mpi(p, id, send_counts , send_index, img);
                break;
            case 7:
                stop=false;
                break;
            default:
                if(id==0)
                {
                    cout<<"please type from 1 to 7!"<<endl;
                }
                break;
        }

    }
    if(id==0)
    {
        cout<<"program ends!"<<endl;
        imshow("image", img);
        waitKey(0);
        // imwrite( "gray_image.jpg", img );
    }
    
    delete[] send_counts;
    delete[] send_index;
    MPI_Finalize();
    return 0;

}