#include <iostream>
#include <cstdio>
#include "mpi.h"
#include "opencv2/opencv.hpp"
#include "img_processing.h"
#include "img_mpi.h"

#define min 1 //Represent the minimum selection number
#define max 8 //Represent the maximum selection number

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    int selection; //Select which img_processing function
    int stop=true; //Stop the program when stop equals to false
    
    Mat img; //Store the input image
    Mat prev_img; //Store the previous modified image
 
    int *send_counts; //Store the size of the assgined sub-image of each process
    int *send_index; //Store the start index of the assgined sub-image of each process

    int min_selection=min; //Represent the minimum selection number
    int max_selection=max; //Represent the maximum selection number

    int p, id; //The number of the processes and thier id
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

        //Initialize the previous image to be the input image
        prev_img=img;
        

        //Display the window infinitely until any keypress
        cout<<"**** Display the window infinitely until any keypress ****"<<endl;
        imshow("image", img);
        waitKey(0);
        destroyAllWindows();
    }

    //Initialize the sending buffers as communication_arrays for MPI
    send_counts = new int[p]; 
    send_index = new int[p]; 
   
    MPI_Barrier(MPI_COMM_WORLD);
    
    //Keep executing various image processing function until exit
    while (stop)
    {
        if(id==0)
        {
            cout<<"Please select:"<<endl<<" 1.Image Zooming"<<endl<<" 2.Image Rotation"<<endl<<" 3.Image Contrast & Brightness"<<endl<<" 4.Image Blurring"<<endl<<" 5.Image Grayscale"
            <<endl<<" 6.Image Saving"<<endl<<" 7.Back to only one previous step"<<endl<<" 8.Exit"<<endl;
        }
        
        selection=get_valid_input<int>(id, min_selection, max_selection);
     
        switch (selection)
        {
            case 1:
                prev_img=img; 
                img_zooming_mpi(p, id, send_counts , send_index, img);
                break;
            case 2:
                prev_img=img; 
                img_rotation_mpi(p, id, send_counts , send_index, img);
                break;
            case 3:
                prev_img=img; 
                img_contrast_brightness_mpi(p, id, send_counts , send_index, img);
                break;
            case 4:
                prev_img=img; 
                img_blurring_mpi(p, id, send_counts , send_index, img);
                break;
            case 5:
                prev_img=img; 
                img_grayscale_mpi(p, id, send_counts , send_index, img);
                break;
            case 6:
                img_saving(id, img);
                break;
            case 7:
                img=prev_img;
                break;
            case 8:
                stop=false;
                break;
            default:
                if(id==0)
                {
                    cout<<"Please type from 1 to 8!"<<endl;
                }
                break;
        }
        
        //The modified image will be shown after every process end
        if(id==0 && selection!=8 and selection!=6)
        {
            cout<<"processing ends!"<<endl;
            cout<<"**** Display the window infinitely until any keypress ****"<<endl;
            imshow("image", img);
            waitKey(0);
            destroyAllWindows();
        }
    }
    if(id==0)
    {
        cout<<"program ends!"<<endl;
    }
    
    delete[] send_counts;
    delete[] send_index;
    MPI_Finalize();
    return 0;
}