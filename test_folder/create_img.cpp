#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <typeinfo> 

using namespace std;
using namespace cv;
 
int main()
{
	// 设置窗口
	// Mat img = Mat::zeros(Size(5, 500), CV_8UC3);
	Mat img(300, 100, CV_8UC3,Scalar(100,100,200));
	// Mat img(300, 500, 16);
	// Mat img(2, 4, CV_8UC3,Scalar(1,2,3));
	// Mat img(2, 4, 0,Scalar(20));
	// Mat img2 = Mat::zeros(Size(2, 4), CV_8UC3);
	// Mat img2(2, 4, CV_8UC3);
	// Mat img2=img.clone();
	// Mat *img2;
	// *img2=img;
	imshow("board", img);
	waitKey();

	std::cout << "img_size " << img.size() << "img_type " << img.type() << std::endl; 
	// cout<<img<<endl;
	cout<<long(img.data)<<endl;
	// cout<<img.at<cv::Vec3b>(1,3)<<endl;
	// cout<<img.at<cv::Vec3b>(3,1)<<endl;
	// cout<<(int)img.at<uchar>(3,1)<<endl;
	// cout<<(int)img.at<uchar>(1,3)<<endl;
	cout<<"rows= "<<img.rows<<" cols= "<<img.cols<<" channels= "<<img.channels()<<" total= "<<img.total()<<endl;
	cout<<"test-loop"<<endl;

	uchar test[90000];
	cout<<long(test)<<endl;
	for(int i=0;i<90000;i++)
	{
		if(i<30000)
		{
			test[i]=0;
		}
		else if(i<60000)
		{
			test[i]=100;
		}
		else
		{
			test[i]=255;
		}
	}
	img.data=test;
	// cout<<img<<endl;

	std::cout << "img_size " << img.size() << "img_type " << img.type() << std::endl; 
	cout<<long(img.data)<<endl;
	cout<<"rows= "<<img.rows<<" cols= "<<img.cols<<" channels= "<<img.channels()<<" total= "<<img.total()<<endl;
	cv::imwrite( "revised_image.jpg", img );

	// //test pixel by for loop
	// int row=img.rows;
	// int channel=img.channels();
	// int col=img.cols*channel;

	// if(img.isContinuous())
	// {
	// 	cout<<"isContinuous"<<endl;
	// }

	// // cout<<(int)img.ptr<uchar>(0)<<" "<<(int)img.ptr<uchar>(0).name()<<endl;
	// // cout<<img.ptr<cv::Vec3b>(0)<<" "<<img.ptr<cv::Vec3b>(0).name()<<endl;
	// // cout<<(int)img.ptr(0)<<" "<<(int)img.ptr(0).name()<<endl;

	// cout<<(long)(img.ptr<uchar>(0))<<endl;
	// cout<<(long)(img.ptr<cv::Vec3b>(0))<<endl;
	// cout<<(long)(img.ptr(0))<<endl;
	// for(int i=0;i<row;i++)
	// {
	// 	uchar* data=img.ptr<uchar>(i);
	// 	// cv::Vec3b* data=img.ptr<cv::Vec3b>(i);
	// 	// cout<<data;
	// 	for(int j=0;j<col;j++)
	// 	{
	// 		cout<<(int)data[j]<<"#";
	// 		data[j]=255;
	// 	}
	// 	cout<<endl;
	// }
	// cout<<img<<endl;

	// uchar* ptr_1d = img.data;
	// cout<<long(ptr_1d)<<" "<<img.total()*img.channels()<<endl;
	// for(int i=0;i<img.rows*img.cols*img.channels();i++)
	// {
	// 	ptr_1d[i]=33;
	// 	cout<<(int)ptr_1d[i]<<" ";
	// }
	// cout<<endl;
	// cout<<img<<endl;
	// std::cout << "img2_size " << img2.size() << "img2_type " << img2.type() << std::endl; 
	// cout<<img2<<endl;
	// cout<<long(img2.data)<<endl;
	
	// if (!cv::norm(img,img2,NORM_L1))
	// {
	// 	printf("yes\n");
	// }
	// else
	// {
	// 	printf("no\n");
	// }

	// std::cout << "img2_size " << (*img2).size() << "img2_type " << (*img2).type() << std::endl; 
	// cout<<*img2<<endl;
	// cout<<long((*img2).data)<<endl;
	
	// if (!cv::norm(img,*img2,NORM_L1))
	// {
	// 	printf("yes\n");
	// }
	// else
	// {
	// 	printf("no\n");
	// }
	
	// img.setTo(255);              // 设置屏幕为白色
	// Point p1(100, 100);          // 点p1
	// Point p2(758, 50);           // 点p2
 
	// // 画直线函数
	// line(img, p1, p2, Scalar(0, 0, 255), 2);   // 红色
	// line(img, Point(300, 300), Point(758, 400), Scalar(0, 255, 255), 3);
 
	// Point p(20, 20);//初始化点坐标为(20,20)
	// circle(img, p, 1, Scalar(0, 255, 0), -1);  // 画半径为1的圆(画点）
 
	// Point p4;
	// p4.x = 300;
	// p4.y = 300;
	// circle(img, p4, 100, Scalar(120, 120, 120), -1);
 
	// int thickness = 3;
	// int lineType = 8;
	// double angle = 30;  //椭圆旋转角度
	// ellipse(img, Point(100, 100), Size(90, 60), angle, 0, 360, Scalar(255, 255, 0), thickness, lineType);
 
 
	// 画矩形
	// Rect r(250, 250, 120, 200);
	// rectangle(img, r, Scalar(0, 255, 255), 3);
 
	imshow("board", img);
	waitKey();
	return 0;
}