// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include "ColorTransformer.h"
#include "Converter.h"
//using namespace cv;

int main()
{
	cv::Mat src = cv::imread("girl.jpg");
	cv::namedWindow("Source Image");
	cv::imshow("Source Image", src);

	cv::Mat hist;
	cv::Mat hsv;
	cv::Mat rgb;

	/*ColorTransformer* ct = new ColorTransformer();
	ct->CalcHistogram(src, hist);*/

	Converter* cvt = new Converter();
	cvt->Convert(src, hsv, 2);
	cvt->Convert(hsv, rgb, 3);

	cv::namedWindow("HSV Image");
	cv::imshow("HSV Image", hsv);

	cv::namedWindow("RGB Image");
	cv::imshow("RGB Image", rgb);
	cv::waitKey(0);
	
	return 0;
}

