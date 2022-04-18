#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
 
using namespace std;
using namespace cv;
 
int main()
{
	// 设置窗口
	Mat img = Mat::zeros(Size(800, 600), CV_8UC3);
	
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
	Rect r(250, 250, 120, 200);
	rectangle(img, r, Scalar(0, 255, 255), 3);
 
	imshow("board", img);
	waitKey();
	return 0;
}