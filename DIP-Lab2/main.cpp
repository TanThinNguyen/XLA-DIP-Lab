// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include "GeometricTransformer.h"
#define PI 3.14159
using namespace std;
using namespace cv;

int main()
{
	//Mat src = imread("flower.png");
	//Mat src = imread("scene.jpg");
	Mat src = imread("girl.jpg");
	namedWindow("Source Image");
	imshow("Source Image", src);

	cout << "SrcRow: " << src.rows << endl;
	cout << "SrcCol: " << src.cols << endl;
	
	Mat dst;
	GeometricTransformer* transformer = new GeometricTransformer();
	//PixelInterpolate* interpolator = new NearestNeighborInterpolate();
	PixelInterpolate* interpolator = new BilinearInterpolate();

	int ret = 0;
	// Phóng to, thu nhỏ
	//ret = transformer->Scale(src, dst, 0.3, 0.3, interpolator);
	//ret = transformer->Scale(src, dst, 1.1, 1.1, interpolator);

	// Thay đổi kích thước
	//ret = transformer->Resize(src, dst, 250, 200, interpolator);
	//ret = transformer->Resize(src, dst, 650, 500, interpolator);

	//src = dst;
	// Xoay ảnh quanh tâm
	//ret = transformer->RotateUnkeepImage(src, dst, 35 * PI / 180, interpolator);
	ret = transformer->RotateKeepImage(src, dst, -60 * PI / 180, interpolator);

	if (ret)
	{
		namedWindow("Dest Image");
		imshow("Dest Image", dst);
	}
	else
	{
		cout << "Khong the thuc hien phep bien doi";
	}

	cout << "DstRow: " << dst.rows << endl;
	cout << "DstCol: " << dst.cols << endl;

	cv::waitKey(0);
	return 0;
}

