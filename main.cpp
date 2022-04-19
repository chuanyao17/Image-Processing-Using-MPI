#include "img_processing.h"
#include <iostream>
#include <cstdio>
#include "mpi.h"
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int argc, char* argv[])
{
    int selection; //select which img_processing function
    int img;
    int stop=true; //stop the program when stop equals to false
    
    Mat image; //store the input image
    
    //check the valid iput
    if ( argc != 2 ) {
    printf("Usage: %s filename\n", argv[0]);
    return -1;
    }
    image = imread( argv[1], 1 );
    if ( !image.data ) {
    printf("No image data \n");
    return -1;
    }
    // std::cout << "img_size " << image.size() << "img_type " << image.type() << std::endl; 
    //show the input image on the screen
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    waitKey(0); // display the window infinitely until any keypress (need to be modified)
    destroyAllWindows();
    // waitKey(1);
    
    printf("program starts:  \n");
    while (stop)
    {
        printf("please select: 1.img_zooming 2.img_rotation 3.img_brightness 4.img_contrast 5.img_blurring 6.img_grayscale 7.exit\n");  
        scanf("%d",&selection); // need to check input , only accept number!
        //considering using enum
        switch (selection)
        {
            case 1:
                img_zooming(image,8,8);
                break;
            case 2:
                break;
            case 3:
                img_brightness(image,1.5,10);
                break;
            case 4:
                img_contrast(image,50.f);
                break;
            case 5:
                img_blurring(image);
                break;
            case 6:
                img_grayscale(image);
                break;
            case 7:
                stop=false;
                break;
            default:
                printf("again \n");
                break;
        }

    }
    printf("program ends!\n");
    return 0;

}