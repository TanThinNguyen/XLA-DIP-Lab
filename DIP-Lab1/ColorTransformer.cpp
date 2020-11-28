#include "ColorTransformer.h"

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix)
{
	if (sourceImage.data == NULL)
	{
		return 0;
	}

	int nChannels = sourceImage.channels();

	// Khởi tạo ma trận histogram (nChannel x 256), giá trị mặc định là 0
	histMatrix = Mat(nChannels, 256, CV_16UC1, Scalar(0));
	int widthStep = histMatrix.step[0];		// khoảng cách byte giữa các dòng
	ushort* pHistData = (ushort*)histMatrix.data;

	// duyệt qua các dòng pixel của ảnh source
	for (int y = 0; y < sourceImage.rows; y++)
	{
		// lấy con trỏ đầu dòng
		const uchar* pRow = sourceImage.ptr<uchar>(y);

		//for (int x = 0; x < sourceImage.cols * nChannels; x++)
		//{
		//	// Nếu nChannel = 1, i = 0, chỉ có 1 kênh 
		//	// Nếu nChannel = 3, i = 0, 1, 2 tương ứng với điểm ảnh thuộc kênh B, G, R
		//	int i = x % nChannels;

		//	/* Đếm số lượng điểm ảnh
		//	nChannel = 1, histMatrix chỉ có 1 dòng
		//	nChannel = 3, histMatrix có các dòng 1, 2, 3 lần lược tương ứng kênh B, G, R
		//	*/
		//	// Công thức truy cập pData[rowIdx*matF.step1() + colIdx]
		//	pHistData[i * widthStep + pRow[x]]++; // (pHistData + i*widthStep)[pRow[x]]++;
		//}

		for (int x = 0; x < sourceImage.cols; x++, pRow += nChannels)
		{
			// Nếu nChannel = 1, i = 0, chỉ có 1 kênh
			// Nếu nChannel = 3, i = 0, 1, 2 tương ứng với điểm ảnh thuộc kênh B, G, R
			if (nChannels == 1)
			{
				pHistData[pRow[0]]++;
			}
			else if (nChannels == 3)
			{
				// Công thức truy cập pData[rowIdx*matF.step1() + colIdx]
				pHistData[pRow[0]]++;
				pHistData[widthStep + pRow[1]]++;
				pHistData[widthStep * 2 + pRow[2]]++;
			}
		}
	}

	return 1;
}

ColorTransformer::ColorTransformer()
{
}

ColorTransformer::~ColorTransformer()
{
}
