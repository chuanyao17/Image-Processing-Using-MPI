#ifndef __MATRIX_PARA_H__
#define __MATRIX_PARA_H__
#include <iostream>
#include "opencv2/opencv.hpp"
#include "mpi.h"


using namespace cv;
using namespace std;

void get_gaussian_kernel(const int &, double*, const double &); //Get the Gaussian kernel for gaussian blurring used
void update_send_index(const int &, int *, int *); //Update send_index based on the calculated send_counts
void update_communication_arrays (const int &, const int &, const int &, const int &, int *, int *); //Initialize communication arrays to pass to MPI gather/scatterv based on the image's row
void update_communication_arrays_opposite(const int &, const int &, const int &, const int &, int *, int *); //Initialize communication arrays to pass to MPI gather/scatterv based on the image's row, but the assigned of the send_counts' index are oppsite than the original fuction
void update_communication_arrays_zoom(const int &, const int &, const Mat &, int *, int *); //Initialize communication arrays based on the zoomed sub-images to pass to MPI gather/scatterv
void update_communication_arrays_border (const int &, const int &, const int &, const int &, int *, int *, const int &); //Initialize communication arrays to pass to MPI gather/scatterv based on the image's row and adding additional border width as offset
void update_communication_arrays_by_col(const int &, const int &, const int &, const int &, int *, int *, const bool); //Initialize communication arrays to pass to MPI gather/scatterv based on the image's col
Mat distribute_image(const int &, const int &, const int &, const int &, const int &, const int *, const int *, const uchar *); //Distribute the input image row-wise among each processor 
Mat distribute_image_by_col(const int &, const int &, const int &, const int &, const int &, const int *, const int *, const uchar *); //Distribute the input image col-wise among each processor 
void update_image_properties(const int &, const Mat &, int &, int &, int &, int &); //Send the image properties from the ROOT to other processes
void print_send_buffers(const int &, const int &, int * , int *, int &, int &); //Check *send_counts and * send_index
void img_grayscale_mpi(const int &, const int &, int *, int *, Mat &); 
void img_contrast_brightness_mpi(const int &, const int &, int *, int *, Mat &); 
void img_zooming_mpi(const int &, const int &, int *, int *, Mat &);
void img_rotation_mpi(const int &, const int &, int *, int *, Mat &);
void img_blurring_mpi(const int &, const int &, int *, int *, Mat &); 



//Send the input of the input stream from the ROOT to other processes
template <typename T>
T get_valid_input(const int &id, T &min, T &max)
{
    T input;
    MPI_Datatype mpi_type = MPI_DATATYPE_NULL;
    //Check if input type is int or double only
    if(is_same<T, int>::value)
    {
        mpi_type = MPI_INT;
    }
    else if(is_same<T, double>::value)
    {
        mpi_type = MPI_DOUBLE;
    }
    else
    {
        cerr<<"only accept int or double using this template!"<<endl;
        assert(mpi_type != MPI_DATATYPE_NULL);
        return 0;
    }
    if(id==0)
    {
        while(true)
        {
            //Get input with type T
            cin >> input;

        //Check if the input is type T, between the right range, and also the input should be the only thing in the input stream, otherwise things like 2b would be a valid
            if ((input<min) || (input>max) || cin.fail() || cin.get() != '\n')
            {
                //Reset the error state flag back to goodbit
                cin.clear();

                //Clear the input stream using and empty while loop
                while (cin.get() != '\n');

                cout<<"Invalid input"<<endl;
                continue;
            }
            break;
        }
    }
    
	MPI_Bcast( &input, 1, mpi_type, 0, MPI_COMM_WORLD);
    return input;
}


#endif