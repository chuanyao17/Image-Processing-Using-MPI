#ifndef __MATRIX_PARA_H__
#define __MATRIX_PARA_H__
#include "opencv2/opencv.hpp"


using namespace cv;


void update_communication_arrays (const int &, const int &, const int &, const int &, int *, int *); //create_communication_arrays() initialize arrays to pass to MPI gather/scatterv
Mat distribute_image(const int &, const int &, const int &, const int &, const int *, const int *, const uchar *); //distribute the input image among each processor 
void update_image_properties(const int &, const Mat &, int &, int &, int &); //Send the image properties from the ROOT to other processes
void print_send_buffers(const int &, const int &, int * , int *); //check *send_counts and * send_index
int read_selection(const int &id); //Send the selection from the ROOT to other processes
Mat img_grayscale_mpi(const int &, const int &, int *, int *, const Mat &); 

#endif