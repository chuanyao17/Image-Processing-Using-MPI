#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "mpi.h"
#include "matrix_para.h"

using namespace cv;
using namespace std;

void create_communication_arrays (int &p, int &img_row_num, int &img_col_num, int &img_ch_num, int *send_counts , int *send_index)
{
    for(int i = 0; i < p; i++) {
        send_counts[i] = ((((i+1)*img_row_num)/p)-((i*img_row_num)/p))*img_col_num*img_ch_num; //The number of elements is assigned rows * image's cols * image's channels
        send_index[i] = (((i*img_row_num)/p))*img_col_num*img_ch_num;
    }
}