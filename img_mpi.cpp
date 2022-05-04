#include <iostream>
#include <cstdio>
#include <algorithm> 
#include <math.h>
#include "opencv2/opencv.hpp"
#include "mpi.h"
#include "img_mpi.h"
#include "img_processing.h"


#define img_minimum_len 1    //Represent the minimum pixel that the image can be zoomed out
#define img_maximum_len 4000 //Represent the maximum pixel that the image can be zoomed out

#define averaging_blur 1    //Represent the selection number of the averaging blur method
#define gaussian_blur 2     //Represent the selection number of the gaussian blur method
#define kernel_min_size 3   //Represent the minimum kernel size
#define kernel_max_size 9   //Represent the maximum kernel size

#define sigma_min_value 0.5 //Represent the minimum sigma value for creating the gaussian kernel
#define sigma_max_value 5   //Represent the maximum sigma value for creating the gaussian kernel

#define alpha_min_value 0.1 //Represent the minimum alpha value for the image contrast ratio
#define alpha_max_value 3   //Represent the maximum alpha value for the image contrast ratio

#define beta_min_value 0    //Represent the minimum beta value for increasing the brightness
#define beta_max_value 100  //Represent the maximum beta value for increasing the brightness

using namespace cv;
using namespace std;

void get_gaussian_kernel(const int &size, double* gaus, const double &sigma)
{
    
    double sum=0.0;    //Sum is for normalization
    int offset=size/2; //The offset of the coordinates along axis from the origin

    //Calculate the Gaussian Kernel based on the Gaussian equation for 2D matrix
    for(int i=0;i<size;i++)
    {
        for(int j=0;j<size;j++)
        { 
            gaus[i*size+j]= (exp(-((i-offset)*(i-offset)+(j-offset)*(j-offset)) / (2*sigma*sigma) )) / (2*sigma*sigma*M_PI);
            sum+=gaus[i*size+j];
        }
    }

    //Normalize the Kernel
    for(int i=0;i<size;i++)
    {
        for(int j=0;j<size;j++)
        {
           gaus[i*size+j]/=sum;
        }
    }
}

void update_send_index(const int &p, int *send_counts , int *send_index)
{
    for(int i=1;i<p;i++)
    {
        send_index[i]=send_index[i-1]+send_counts[i-1]; //Update send_index by send_counts
    }
}

void update_communication_arrays(const int &p, const int &img_row_num, const int &img_col_num, const int &img_ch_num, int *send_counts , int *send_index)
{
    
    int sub_row_num;
    int elements_num=img_col_num*img_ch_num; //Represent the total elements of a row
    
    for(int i = 0; i < p; i++) {
        sub_row_num=((((i+1)*img_row_num)/p)-((i*img_row_num)/p));
        send_counts[i] = sub_row_num*elements_num; //The number of elements is sub-rows * image's cols * image's channels
        send_index[i] = (((i*img_row_num)/p))*elements_num;
    }
}

void update_communication_arrays_border(const int &p, const int &img_row_num, const int &img_col_num, const int &img_ch_num, int *send_counts , int *send_index, const int &border)
{
    int sub_row_num;
    int elements_num=img_col_num*img_ch_num; //Represent the total elements of a row
    int first; //Store the index of the first processor which contain non-zero assigned sub-rows 
    int last;   //Store the index of the last processor which contain non-zero assigned sub-rows 
    bool first_non_zeron_found=false;
    bool last_non_zero_found=false;
    for(int i=0;i<p;i++)
    {
        //Move index to the previous(upper) row based on the border width except the first processor which contain non-zero assigned sub-rows
        if(send_index[i]!=0)
        {
            send_index[i]-=(elements_num*border);
            
        }

        //Add back the additional border elements to all the processor which contain non-zero assigned sub-rows
        if(send_counts[i]!=0)
        {
            send_counts[i]+=(elements_num*border*2);
        }
    }
}   



Mat distribute_image(const int &id, const int &img_row_num, const int &img_col_num, const int &img_ch_num, const int &img_type, const int *send_counts , const int *send_index, const uchar *img_data)
{   
    int recv_counts=send_counts[id]; //Store the size of the sub-image's data
    Mat sub_img(recv_counts/(img_col_num*img_ch_num),img_col_num, img_type); //Construct the sub-image with (assigned rows, image's col number, 3 channels)
    //Assign the sublist from process 0 to each process
    MPI_Scatterv(img_data, send_counts, send_index, MPI_UNSIGNED_CHAR, sub_img.data, recv_counts, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    return sub_img;
}

Mat distribute_image_full(const int &id, const int &img_row_num, const int &img_col_num, const int &img_ch_num, const int &img_type, const Mat &img)
{   
    
    Mat sub_img=Mat( img_row_num, img_col_num, img_type); //Construct the sub-image as same as the input image
    if(id==0)
    {
        sub_img=img.clone(); //Clone the input image the the sub-image
    }
    //Broadcast the sub-image to other processor from the ROOT
    MPI_Bcast(sub_img.data, img_row_num*img_col_num*img_ch_num, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD );
    return sub_img;
}

void update_image_properties(const int &id, const Mat &img, int &img_row_num, int &img_col_num, int &img_ch_num, int &img_type)
{
    if(id==0)
    {
        img_row_num=img.rows;
        img_col_num=img.cols;
        img_ch_num=img.channels();
        img_type=img.type();
    }
    //Broadcast the input image properties to other processor from the ROOT
    MPI_Bcast( &img_row_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &img_col_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &img_ch_num, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &img_type, 1, MPI_INT, 0, MPI_COMM_WORLD );
}

void print_send_buffers(const int &id, const int &p, int *send_counts , int *send_index, int &img_col_num, int &img_ch_num)
{
    if(id==0)
    {
        cout<<"send_counts= ";
        for(int i=0;i<p;i++)
        {
            cout<<send_counts[i]<<" ";
        }
        cout<<endl;
        cout<<"send_index= ";
        for(int i=0;i<p;i++)
        {
            cout<<send_index[i]<<" ";
        }
        cout<<endl;
        cout<<"send_row= ";
        for(int i=0;i<p;i++)
        {
            cout<<send_counts[i]/(img_col_num*img_ch_num)<<" ";
        }
        cout<<endl;
    }
}

void img_grayscale_mpi(const int &p, const int &id, int *send_counts , int *send_index, Mat &img)
{
    if(id==0)
    {
        printf("img_grayscale is working\n");
    }

    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel
    int img_type; //Store the input image's type
	Mat sub_img; //Store the distributed sub-image

    
	update_image_properties(id, img, img_row_num, img_col_num, img_ch_num, img_type);
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    
    //Distribute the whole input image to the sub-images for each processor
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, img_type, send_counts, send_index, img.data);

    //Execute the image processing function based on the assigned image
    sub_img=img_grayscale(sub_img);
    
    //Initialize the image and gather all the modified sub-images to be a full modified image
    if(id==0)
    {
        img = Mat( img_row_num, img_col_num, img.type());
    }
    MPI_Gatherv(sub_img.data, send_counts[id], MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    return;
}

void img_contrast_brightness_mpi(const int &p, const int &id, int *send_counts , int *send_index, Mat &img)
{
    if(id==0)
    {
        printf("img_grayscale is working\n");
    }

    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel
    int img_type; //Store the input image's type
	Mat sub_img; //Store the distributed sub-image

    double alpha; //Represent the image contrast ratio
    double beta;  //Represent the increasing image brightness

    double alpha_min=alpha_min_value;
    double alpha_max=alpha_max_value;

    double beta_min=beta_min_value;
    double beta_max=beta_max_value;

    if(id==0)
    {
        cout<<"Please input a number between "<<alpha_min<<" to "<<alpha_max<<" as the constrast ratio"<<endl;
    }
    alpha=get_valid_input<double>(id, alpha_min, alpha_max);

    if(id==0)
    {
        cout<<"Please input a number between "<<beta_min<<" to "<<beta_max<<" as the increasing brightness"<<endl;
    }
    beta=get_valid_input<double>(id, beta_min, beta_max);
    

	update_image_properties(id, img, img_row_num, img_col_num, img_ch_num, img_type);
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
	
    //Distribute the whole input image to the sub-images for each processor
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, img_type, send_counts, send_index, img.data);
    
    //Execute the image processing function based on the assigned image
    sub_img=img_contrast_brightness(sub_img, alpha, beta);

    //Initialize the image and gather all the modified sub-images to be a full modified image
    if(id==0)
    {
        img = Mat( img_row_num, img_col_num, img.type());
    }
    MPI_Gatherv(sub_img.data, send_counts[id], MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    return;
}

void img_zooming_mpi(const int &p, const int &id, int *send_counts , int *send_index, Mat &img)
{
    if(id==0)
    {
        printf("img_zooming is working\n");
    }
    

    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel
    int img_type; //Store the input image's type
	Mat sub_img; //Store the distributed sub-image

    double height_ratio; //Represent the zooming ratio of the row axis
    double width_ratio; //Represent the zooming ratio of the col axis
    double min_height_ratio, max_height_ratio;
    double min_width_ratio, max_width_ratio;

    update_image_properties(id, img, img_row_num, img_col_num, img_ch_num, img_type);
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    
    //Set the min height ratio to be 1 if the estimated height ratio is bigger than 1
    //The estimated height ratio will based on the number of processors to prevent from the situation like the processor only owns one row but need to times the ratio smaller than 1
    min_height_ratio=min(((double)p/img_row_num),1.0);
    //Set the max height ratio based on the img_maximum_len
    max_height_ratio=(double)img_maximum_len/img_row_num;
    
    //Strict the maximum the image size for zoom in
    if(img_row_num>img_maximum_len)
    {
        max_height_ratio=1.0;
    }

    //Set the min height ratio to be 1 if the estimated width ratio is bigger than 1
    //The estimated width ratio will based on the img_minimum_len which restrict the minimum size of the zoom out image to be 1
    min_width_ratio=min(((double)img_minimum_len/img_col_num),1.0);
    //Set the max width ratio based on the img_maximum_len
    max_width_ratio=(double)img_maximum_len/img_col_num;
    
    //Strict the maximum the image size for zoom in
    if(img_col_num>img_maximum_len)
    {
        max_width_ratio=1.0;
    }
    
    if(id==0)
    {
        cout<<"Please input a number between "<<min_height_ratio<<" to "<<max_height_ratio<<" as the zooming ratio of the row axis"<<endl;
    }
    height_ratio=get_valid_input<double>(id, min_height_ratio, max_height_ratio);
    
    if(id==0)
    {
        cout<<"Please input a number between "<<min_width_ratio<<" to "<<max_width_ratio<<" as the zooming ratio of the col axis"<<endl;
    }
    width_ratio=get_valid_input<double>(id, min_width_ratio, max_width_ratio );
  	


	//Distribute the whole input image to the sub-images for each processor
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, img_type, send_counts, send_index, img.data);

    //Execute the image processing function based on the assigned image
    sub_img=img_zooming(sub_img, height_ratio, width_ratio);

    //Update communication arrays by each sub_img
    send_counts[id]=sub_img.rows*sub_img.cols*sub_img.channels();
    MPI_Allgather(&send_counts[id], 1, MPI_INT, send_counts, 1, MPI_INT, MPI_COMM_WORLD);
    update_send_index(p, send_counts , send_index);

    // Reset the input image rows based on the sub-images
    MPI_Allreduce(&sub_img.rows, &img_row_num, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    // Reset the input image cols based on the original input cols times the width ratio
    img_col_num=img_col_num*width_ratio;
    
    //Initialize the image and gather all the modified sub-images to be a full modified image
    if (id==0)
    {
        img = Mat( img_row_num, img_col_num, img.type());
    }
    MPI_Gatherv(sub_img.data, send_counts[id], MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    return;
}

void img_rotation_mpi(const int &p, const int &id, int *send_counts , int *send_index, Mat &img)
{
    if(id==0)
    {
        printf("img_rotation\n");
    }

    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel
    int img_type; //Store the input image's type
    bool clock_wise=false;
    int dir_selection; //Store the select direction 
    int clockwise=1; //Represent the selection of clockwise to be 1
    int counter_clockwise=2; //Represent the selection of counter clockwise to be 2
	Mat sub_img; //Store the distributed sub-image

    if(id==0)
    {
        cout<<"Please select direction: 1. clockwise 2. counter-clockwise"<<endl;
    }
    dir_selection=get_valid_input<int>(id,clockwise,counter_clockwise);
    if(dir_selection==clockwise)
    {
        clock_wise=true;
    }
	update_image_properties(id, img, img_row_num, img_col_num, img_ch_num, img_type);
    update_communication_arrays (p, img_col_num, img_row_num, img_ch_num, send_counts , send_index);
	
    //Broadcast the whole input image to each processor
    sub_img=distribute_image_full(id, img_row_num, img_col_num, img_ch_num, img_type, img);
   
    //Execute the image processing function based on the assigned image
    sub_img=img_rotation(sub_img, send_counts[id]/(img_row_num*img_ch_num), img_row_num, send_index[id]/(img_row_num*img_ch_num), clock_wise);
    
    //Initialize the image and gather all the modified sub-images to be a full modified image
    if (id==0)
    {
        img = Mat( img_col_num, img_row_num, img.type()); //Rows and cols are swapped because of the rotation
    }
    MPI_Gatherv(sub_img.data, send_counts[id], MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    return;
}

void img_blurring_mpi(const int &p, const int &id, int *send_counts , int *send_index, Mat &img)
{
    
    int blur_method; //Represent the selection of the blur method
    int method_start=averaging_blur; //Represent the first number of the selection of the blur method
    int method_end=gaussian_blur;    //Represent the last number of the selection of the blur method
    
    int blur_kernel_size; //Store the size of the kernel
    int kernel_size_start=kernel_min_size/2; //Represent the first number of the selection of the kernel size
    int kernel_size_end=kernel_max_size/2;  //Represent the last number of the selection of the kernel size

    int border_width; //Store the border width to be added around the input image

    double sigma; //Store the sigma value for creating the gaussian kernel
    double sigma_min=sigma_min_value;
    double sigma_max=sigma_max_value;

    double *gaus; //Store the Gaussian kernel
    
    if(id==0)
    {
        cout<<"Please select blurring method: 1. Averaging Blur 2. Gaussian Blur"<<endl;
    }
    blur_method=get_valid_input<int>(id,method_start,method_end);

    if(id==0 && blur_method==gaussian_blur)
    {
        cout<<"Please select the value of sigma from 0.5 to 5, the higer value the better blurring effect"<<endl;
    }
    if(blur_method==gaussian_blur)
    {
        sigma=get_valid_input<double>(id,sigma_min,sigma_max);
    }
    
    if(id==0)
    {
        cout<<"Please select the size of the blur kernal: 1. 3*3  2. 5*5  3. 7*7 4. 9*9 "<<endl;
    }
    border_width=get_valid_input<int>(id,kernel_size_start,kernel_size_end);
    
    //Set the kernel size based on the border width
    blur_kernel_size= border_width*2+1;
    
    //If the gaussiain blur is selected, allocate the memory to it and calculate the gaussian kernel
    if(blur_method==gaussian_blur)
    {
        gaus=new double[blur_kernel_size*blur_kernel_size];
        get_gaussian_kernel(blur_kernel_size, gaus, sigma);
    }
    
    
    //Added the border to the input image by the ROOT processor for future matrix convolution used
    if(id==0)
    {
        printf("img_blurring is working\n");
        copyMakeBorder(img, img, border_width, border_width, border_width, border_width, BORDER_REFLECT_101); //Create a border around the image
    }

    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel
    int img_type; //Store the input image's type
	Mat sub_img; //Store the distributed sub-image

    
	update_image_properties(id, img, img_row_num, img_col_num, img_ch_num, img_type);
    
    //Reset the number image row before the borders are added
    img_row_num-=(border_width*2);
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);

    //Calculate the new communication array 
    update_communication_arrays_border(p, img_row_num, img_col_num, img_ch_num, send_counts , send_index, border_width);
    
    //Distribute the whole input image to the sub-images for each processor
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, img_type, send_counts, send_index, img.data);
    
    //Execute the image processing function based on the assigned image
    sub_img=img_blurring(sub_img, id, p, border_width, gaus ,blur_method);

    //Reset the number image col before the borders are added
    img_col_num-=(border_width*2);

    //Initialize the image and gather all the modified sub-images to be a full modified image
    if(id==0)
    {
        img = Mat( img_row_num, img_col_num, img.type());
    }
    //Reset the communication arrays for gathering all the modified sub-images
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    MPI_Gatherv(sub_img.data, send_counts[id], MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    
    //If the gaussiain blur is selected, delete the allocated memory 
    if(blur_method==gaussian_blur)
    {
        delete[] gaus;
    }
    return;
}