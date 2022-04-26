#ifndef __MATRIX_PARA_H__
#define __MATRIX_PARA_H__
#include "opencv2/opencv.hpp"

using namespace cv;


void create_communication_arrays (int &, int &, int &, int &, int *, int *); //create_communication_arrays() initialize arrays to pass to MPI gather/scatterv
uchar* distribute_image(int &, int &, int &, int &, int &, int * , int *, int &, uchar *); //distribute the input image among each processor 
void update_image_properties(int &, Mat &, int &, int &, int &); //Send the image properties from the ROOT to other processes
void print_send_buffers(int &, int &, int * , int *); //check *send_counts and * send_index

#endif