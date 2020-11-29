// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include "ColorTransformer.h"
#include "Converter.h"
using namespace cv;
using namespace std;

int main()
{
	Mat src = imread("flower.png");
	//Mat src = imread("scene.jpg");
	namedWindow("Source Image");
	imshow("Source Image", src);

	Mat hist;
	Mat hsv;
	Mat rgb;

	/*Converter* cvt = new Converter();
	cvt->Convert(src, hsv, 2);
	cvt->Convert(hsv, rgb, 3);

	cv::namedWindow("HSV Image");
	cv::imshow("HSV Image", hsv);

	cv::namedWindow("RGB Image");
	cv::imshow("RGB Image", rgb);*/

	ColorTransformer* ct = new ColorTransformer();
	ct->CalcHistogram(src, hist);

	cout << "Rows: " << hist.rows << endl;
	cout << "Cols: " << hist.cols << endl;
	int nChannel = hist.channels();

	// Xuất hist
	/*for (int y = 0; y < hist.rows; y++)
	{
		uint32_t* pRow = hist.ptr<uint32_t>(y);

		cout << "[y = " << y << "] ";
		for (int x = 0; x < hist.cols; x++)
		{
			cout << pRow[x] << " ";
		}
		cout << endl;
	}*/

	uint* pHist = (uint*)hist.data;
	for (int y = 0; y < hist.rows; y++, pHist += hist.step1())
	{
		cout << "[y = " << y << "] ";
		uint* pRow = pHist;
		for (int x = 0; x < hist.cols; x++)
		{
			cout << pRow[x] << " ";
		}
		cout << endl;
	}

	cv::waitKey(0);
	
	return 0;
}

