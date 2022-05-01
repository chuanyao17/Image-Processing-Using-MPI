#ifndef __MATRIX_PARA_H__
#define __MATRIX_PARA_H__
#include <iostream>
#include "opencv2/opencv.hpp"
#include "mpi.h"


using namespace cv;
using namespace std;


void update_communication_arrays (const int &, const int &, const int &, const int &, int *, int *); //Initialize communication arrays to pass to MPI gather/scatterv
void update_communication_arrays_border (const int &, const int &, const int &, const int &, int *, int *, const int &); //Initialize communication arrays to pass to MPI gather/scatterv with border width as offset
Mat distribute_image(const int &, const int &, const int &, const int &, const int &, const int *, const int *, const uchar *); //distribute the input image among each processor 
Mat distribute_image_full(const int &, const int &, const int &, const int &, const int &, const Mat &); //distribute the whole input image to each processor 
void update_image_properties(const int &, const Mat &, int &, int &, int &, int &); //Send the image properties from the ROOT to other processes
void print_send_buffers(const int &, const int &, int * , int *); //check *send_counts and * send_index
void img_grayscale_mpi(const int &, const int &, int *, int *, Mat &); 
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
            // cout<<" min= "<<min<<" "<<(input<min)<<" max= "<<max<<endl;

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
            //  cout<<"!!!!!!!!!!!!!!!"<<" min= "<<min<<" "<<(input<min)<<" max= "<<max<<endl;
            break;
        }
    }
    
	MPI_Bcast( &input, 1, mpi_type, 0, MPI_COMM_WORLD);
    return input;
}

#endif