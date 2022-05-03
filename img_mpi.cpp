#include <iostream>
#include <cstdio>
#include <algorithm> 
#include <math.h>
#include "opencv2/opencv.hpp"
#include "mpi.h"
#include "img_mpi.h"
#include "img_processing.h"


#include <typeinfo>

#define img_minimum_len 1
#define img_maximum_len 4000
// #define border_width 1
// #define border_width 2
#define averaging_blur 1
#define gaussian_blur 2
#define kernel_min_size 3
#define kernel_max_size 9

#define sigma_min_value 0.5
#define sigma_max_value 5

#define alpha_min_value 0.1
#define alpha_max_value 3

#define beta_min_value 0
#define beta_max_value 100

using namespace cv;
using namespace std;

void get_gaussian_kernel(const int &size, double* gaus, const double &sigma)
{
    
    double sum=0.0;

    int offset=size/2;

    for(int i=0;i<size;i++)
    {
        for(int j=0;j<size;j++)
        { 
            gaus[i*size+j]= (exp(-((i-offset)*(i-offset)+(j-offset)*(j-offset)) / (2*sigma*sigma) )) / (2*sigma*sigma*M_PI);
            sum+=gaus[i*size+j];
        }
    }

    for(int i=0;i<size;i++)
    {
        for(int j=0;j<size;j++)
        {
           gaus[i*size+j]/=sum;
        //    printf("gaus[%d]=%lf ", (i*size+j), gaus[i*size+j]);
        }
        // printf("\n");
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
    // cout<<"total row= "<<img_row_num<<endl;
    int sub_row_num;
    int elements_num=img_col_num*img_ch_num;
    for(int i = 0; i < p; i++) {
        sub_row_num=((((i+1)*img_row_num)/p)-((i*img_row_num)/p));
        
        // send_counts[i] = ((((i+1)*img_row_num)/p)-((i*img_row_num)/p))*img_col_num*img_ch_num; //The number of elements is assigned sub-rows * image's cols * image's channels
        // send_index[i] = (((i*img_row_num)/p))*img_col_num*img_ch_num;
        send_counts[i] = sub_row_num*elements_num; //The number of elements is sub-rows * image's cols * image's channels
        send_index[i] = (((i*img_row_num)/p))*elements_num;
        
        // cout<<" check= "<<sub_row_num*img_col_num*img_ch_num<<endl;
    }
    
}

void update_communication_arrays_border(const int &p, const int &img_row_num, const int &img_col_num, const int &img_ch_num, int *send_counts , int *send_index, const int &border)
{
    // cout<<"total row= "<<img_row_num<<"~~~~~~border"<<endl;

    int sub_row_num;
    int elements_num=img_col_num*img_ch_num;
    // for(int i = 0; i < p; i++) {
    //     sub_row_num=((((i+1)*img_row_num)/p)-((i*img_row_num)/p));
    //     if(sub_row_num==0)
    //     {
    //         continue;
    //     }
    //     send_counts[i] = ((((i+1)*img_row_num)/p)-((i*img_row_num)/p)+(border*2))*elements_num; //The number of elements is (assigned sub-rows+border*2) * image's cols * image's channels
    // }
    
    int first;
    int last;
    bool first_non_zeron_found=false;
    bool last_non_zero_found=false;
    for(int i=0;i<p;i++)
    {
        //Move index to the previous(upper) row based on the border width except the first processor which contain non-zero assigned sub-rows
        // printf("**************************** send_counts[%d]=%d ****************************\n",i,send_counts[i]);
        if(send_index[i]!=0)
        {
            send_index[i]-=(elements_num*border);
            
        }

        //Add back the additional border elements to all the processor which contain non-zero assigned sub-rows
        
        if(send_counts[i]!=0)
        {
            send_counts[i]+=(elements_num*border*2);
            
           
            // if(!first_non_zeron_found)
            // {
            //     first_non_zeron_found=true;
            //     first=i;
            //     cout<<"first= "<<first<<endl;
            // }
            
        }
        
        // if(send_counts[i]!=0 && !first_non_zeron_found)
        // {
        //     first_non_zeron_found=true;
        //     first=i;
        //     send_counts[first]+=(elements_num*border);
        // }
        // if(send_counts[p-i-1]!=0 && !last_non_zero_found)
        // {
        //     cout<<" last found "<<(p-i-1) <<" current i is "<<i<<endl;
        //     last_non_zero_found=true;
        //     last=p-i-1;
        //     cout<<"last= "<<last<<endl;
        // }
        // printf("send_counts[%d]=%d\n",i,send_counts[i]);
     
    }
    //Remove one side of the border from the first and the last processor which contain non-zero assigned sub-rows if they are not in same index
    // if (first!=last)
    // {
    //     send_counts[first]-=(elements_num*border);
    //     send_counts[last]-=(elements_num*border);
    // }
    
    //Add back the additional border elements if there is only one processor contains non-zero assigned sub-rows
    // if(first==last)
    // {
    //     cout<<"only one!!!!!!!!!!!!!!!!!!!!!"<<endl;
    //     send_counts[first]+=(elements_num*border*2);
    // }

    // // update_send_index(p, send_counts , send_index);
    
}   



Mat distribute_image(const int &id, const int &img_row_num, const int &img_col_num, const int &img_ch_num, const int &img_type, const int *send_counts , const int *send_index, const uchar *img_data)
{   
    
    
    
    // update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    int recv_counts=send_counts[id]; //Store the size of the sub image's data
    // uchar *sub_img_buffer=new uchar[recv_counts]; 
    
    Mat sub_img(recv_counts/(img_col_num*img_ch_num),img_col_num, img_type); //Construct the sub image with (assigned rows, image's col number, 3 channels)
    
    //Assign the sublist from process 0 to each process
    MPI_Scatterv(img_data, send_counts, send_index, MPI_UNSIGNED_CHAR, sub_img.data, recv_counts, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    // sub_img.data=sub_img_buffer;
    // delete[] sub_img_buffer;
    return sub_img;
}

Mat distribute_image_full(const int &id, const int &img_row_num, const int &img_col_num, const int &img_ch_num, const int &img_type, const Mat &img)
{   
    
    Mat sub_img=Mat( img_row_num, img_col_num, img_type);
    if(id==0)
    {
        sub_img=img.clone();
    }
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
        // cout<<" img_row_num= "<<img_row_num<<" img_col_num= "<<img_col_num<<" img_ch_num= "<<img_ch_num<<endl;
    }
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
	// print_send_buffers(id, p, send_counts , send_index, img_col_num, img_ch_num);
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, img_type, send_counts, send_index, img.data);
    // cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<" id "<<id <<endl;
    // imshow("image", sub_img);
    // waitKey(0);
    // destroyAllWindows();
    sub_img=img_grayscale(sub_img);
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

    double alpha;
    double beta;

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
	// print_send_buffers(id, p, send_counts , send_index, img_col_num, img_ch_num);
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, img_type, send_counts, send_index, img.data);
    // cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<" id "<<id <<endl;
    // imshow("image", sub_img);
    // waitKey(0);
    // destroyAllWindows();
    sub_img=img_contrast_brightness(sub_img, alpha, beta);
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

    double height_ratio; //Store the input as the zooming ratio of the row axis
    double width_ratio; //Store the input as the zooming ratio of the col axis
    double min_height_ratio, max_height_ratio;
    double min_width_ratio, max_width_ratio;

    update_image_properties(id, img, img_row_num, img_col_num, img_ch_num, img_type);
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    // print_send_buffers(id, p, send_counts , send_index, img_col_num, img_ch_num);
    min_height_ratio=min(((double)p/img_row_num),1.0);
    max_height_ratio=(double)img_maximum_len/img_row_num;
    if(img_row_num>img_maximum_len)
    {
        max_height_ratio=1.0;
    }
    min_width_ratio=min(((double)img_minimum_len/img_col_num),1.0);
    max_width_ratio=(double)img_maximum_len/img_col_num;
    if(img_col_num>img_maximum_len)
    {
        max_width_ratio=1.0;
    }
    
    
    if(id==0)
    {
        cout<<"Please input a number between "<<min_height_ratio<<" to "<<max_height_ratio<<" as the zooming ratio of the row axis"<<endl;
    }
    
    height_ratio=get_valid_input<double>(id, min_height_ratio, max_height_ratio);
    

    // if(height_ratio<((double)p/img_row_num) || height_ratio>(img_maximum_len/img_row_num))
    // {
    //     if(id==0)
    //     {
    //         cout<<"Invalid height ratio"<<endl;
    //     }
    //     return;
    // }

    if(id==0)
    {
        cout<<"Please input a number between "<<min_width_ratio<<" to "<<max_width_ratio<<" as the zooming ratio of the col axis"<<endl;
    }
    
    
    width_ratio=get_valid_input<double>(id, min_width_ratio, max_width_ratio );
    
    // if(width_ratio< ((double)img_minimum_len/img_col_num)|| width_ratio>(img_maximum_len/img_col_num))
    // {
    //     if(id==0)
    //     {
    //         cout<<"Invalid width ratio"<<endl;
    //     }
    //     return;
    // }
    // cout<<"height_ratio= "<<height_ratio<<" width_ratio= "<<width_ratio<<" id= "<<id<<endl;

	


	
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, img_type, send_counts, send_index, img.data);
    // cout<<"*******"<<"img_row_num= "<<img_row_num<<" img_col_num= "<<img_col_num<<" sub_row_num= "<<sub_img.rows<<" sub_col_num= "<<sub_img.cols<<" total= "<<sub_img.rows*sub_img.cols*sub_img.channels()<<" id= "<<id<<endl;
    
    // cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<" id "<<id <<endl;
    // imshow("image", sub_img);
    // waitKey(0);
    // destroyAllWindows();
    sub_img=img_zooming(sub_img, height_ratio, width_ratio);

    //Update communication arrays by each sub_img
    send_counts[id]=sub_img.rows*sub_img.cols*sub_img.channels();
    MPI_Allgather(&send_counts[id], 1, MPI_INT, send_counts, 1, MPI_INT, MPI_COMM_WORLD);
    update_send_index(p, send_counts , send_index);
    // print_send_buffers(id, p, send_counts , send_index, img_col_num, img_ch_num);
     // Reset the input image to the right size 
    MPI_Allreduce(&sub_img.rows, &img_row_num, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    img_col_num=img_col_num*width_ratio;
    // cout<<"img_row_num= "<<img_row_num<<" img_col_num= "<<img_col_num<<" sub_row_num= "<<sub_img.rows<<" sub_col_num= "<<sub_img.cols<<" total= "<<sub_img.rows*sub_img.cols*sub_img.channels()<<" id= "<<id<<endl;
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
    int dir_selection;
    int clockwise=1;
    int counter_clockwise=2;
    // int prev_row; //Store the number of the previous sub image's row
    
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
	

    sub_img=distribute_image_full(id, img_row_num, img_col_num, img_ch_num, img_type, img);
	// sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, send_counts, send_index, img.data);
    
    // cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<" id "<<id <<endl;
    
    // destroyAllWindows();
    // cout<<"sub_row= "<<send_counts[id]/(img_row_num*img_ch_num)<<" sub_col= "<<img_row_num<<" clock_wise= "<<clock_wise<<" id "<<id <<endl;
    
    // if(id==0)
    // {
    //     prev_row=0;
    // }
    // else
    // {
    //     prev_row=send_counts[id-1]/(img_row_num*img_ch_num);
    // }
    // print_send_buffers(id, p, send_counts , send_index);
   
    sub_img=img_rotation(sub_img, send_counts[id]/(img_row_num*img_ch_num), img_row_num, send_index[id]/(img_row_num*img_ch_num), clock_wise);
    if (id==0)
    {
        img = Mat( img_col_num, img_row_num, img.type());
    }
    
    // string tmp;
    // tmp+=to_string(id);

    // imshow(tmp, sub_img);
    // waitKey(0);
    // cout<<"img_row_num= "<<img_row_num<<" img_col_num= "<<img_col_num<<" sub_row_num= "<<sub_img.rows<<" sub_col_num= "<<sub_img.cols<<" total= "<<sub_img.rows*sub_img.cols*sub_img.channels()<<" id= "<<id<<endl;
    // print_send_buffers(id, p, send_counts , send_index);
    MPI_Gatherv(sub_img.data, send_counts[id], MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    return;
}

void img_blurring_mpi(const int &p, const int &id, int *send_counts , int *send_index, Mat &img)
{
    
    int blur_method;
    int method_start=averaging_blur;
    int method_end=gaussian_blur;
    
    int blur_kernel_size;
    int kernel_size_start=kernel_min_size/2;
    int kernel_size_end=kernel_max_size/2;

    int border_width;

    double sigma_min=sigma_min_value;
    double sigma_max=sigma_max_value;
    double sigma;

    double *gaus;
    
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
    blur_kernel_size= border_width*2+1;
    
    if(blur_method==gaussian_blur)
    {
        gaus=new double[blur_kernel_size*blur_kernel_size];
        get_gaussian_kernel(blur_kernel_size, gaus, sigma);
    }
    
    
    
    if(id==0)
    {
        printf("img_blurring is working\n");
        // cout<<"              " << "size " << img.size()<< " row " << img.rows<< " col " << img.cols   << " img_type " << img.type() <<" id "<<id <<endl;
        copyMakeBorder(img, img, border_width, border_width, border_width, border_width, BORDER_REFLECT_101); //Create a border around the image
        // imshow("image", img);
        // waitKey(0);
        // cout<<" added border " << "size " << img.size()<< " row " << img.rows<< " col " << img.cols   << " img_type " << img.type() <<" id "<<id <<endl;
    }

    int img_row_num; //Store the number of the input image's row
    int img_col_num; //Store the number of the input image's col
    int img_ch_num; //Store the number of the input image's channel
    int img_type; //Store the input image's type
	Mat sub_img; //Store the distributed sub-image

    
    
	update_image_properties(id, img, img_row_num, img_col_num, img_ch_num, img_type);
    
    img_row_num-=(border_width*2);
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);
    // print_send_buffers(id, p, send_counts , send_index, img_col_num, img_ch_num);
    
     

    update_communication_arrays_border(p, img_row_num, img_col_num, img_ch_num, send_counts , send_index, border_width);
    // print_send_buffers(id, p, send_counts , send_index, img_col_num, img_ch_num);
	// cout<<"*******"<<"img_row_num= "<<img_row_num<<" img_col_num= "<<img_col_num<<" id= "<<id<<endl;
	sub_img=distribute_image(id, img_row_num, img_col_num, img_ch_num, img_type, send_counts, send_index, img.data);
    // cout << "sub_size " << sub_img.size()<< " sub_row " << sub_img.rows<< " sub_col " << sub_img.cols   << " sub_img_type " << sub_img.type() <<" id "<<id <<endl;
    // imshow("image", sub_img);
    // waitKey(0);
    // destroyAllWindows();
    sub_img=img_blurring(sub_img, id, p, border_width, gaus ,blur_method);

    img_col_num-=(border_width*2);
    if(id==0)
    {
        img = Mat( img_row_num, img_col_num, img.type());
    }
    update_communication_arrays (p, img_row_num, img_col_num, img_ch_num, send_counts , send_index);

    MPI_Gatherv(sub_img.data, send_counts[id], MPI_UNSIGNED_CHAR, img.data, send_counts, send_index, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    
    if(blur_method==gaussian_blur)
    {
        delete[] gaus;
    }
    

    return;
}