// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "opencv2/opencv.hpp"
//using namespace cv;

int main()
{
	cv::Mat src = cv::imread("girl.jpg");
	cv::namedWindow("Source Image");
	cv::imshow("Source Image", src);

	
	cv::waitKey(0);

	return 0;
}

