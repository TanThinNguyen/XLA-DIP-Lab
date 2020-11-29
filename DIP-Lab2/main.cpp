// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include "GeometricTransformer.h"
using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("girl.jpg");
	namedWindow("Source Image");
	imshow("Source Image", src);
	
	Mat dst;
	GeometricTransformer* geoTrans = new GeometricTransformer();
	PixelInterpolate* interpolator = new NearestNeighborInterpolate();
	geoTrans->Scale(src, dst, 1.5, 1.5, interpolator);

	namedWindow("Dest Image");
	imshow("Dest Image", dst);

	cv::waitKey(0);
	return 0;
}

