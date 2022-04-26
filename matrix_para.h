#ifndef __MATRIX_PARA_H__
#define __MATRIX_PARA_H__
#include "opencv2/opencv.hpp"

using namespace cv;


void create_communication_arrays (int &, int &, int &, int &, int *, int *); //create_communication_arrays() initialize arrays to pass to MPI gather/scatterv
uchar* distribute_image(int &, int &, int &, int &, int &, int * , int *, int &, uchar *);//distribute the input image among each processor 

#endif