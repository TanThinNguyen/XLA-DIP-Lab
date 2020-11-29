#include "ColorTransformer.h"

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix)
{
	if (sourceImage.data == NULL)
	{
		return 0;
	}

	// Thông số ảnh sourceImage
	int rows = sourceImage.rows;
	int cols = sourceImage.cols;
	int nChannels = sourceImage.channels();

	// Khởi tạo ma trận histogram (nChannel x 256), giá trị mặc định là 0
	histMatrix = Mat(nChannels, 256, CV_32SC1, Scalar(0));

	// duyệt qua các dòng pixel của ảnh sourceImage
	for (int y = 0; y < rows; y++)
	{
		// lấy con trỏ đầu dòng của sourceImage
		const uchar* pRow = sourceImage.ptr<uchar>(y);

		for (int x = 0; x < cols; x++, pRow += nChannels)
		{
			for (int k = 0; k < nChannels; k++)
			{
				// lấy con trỏ đầu dòng (tương ứng từng kênh) của histMatrix
				uint* pHistRow = histMatrix.ptr<uint>(k);
				pHistRow[pRow[k]]++;
			}
		}
	}

	return 1;

	/*int height = sourceImage.rows;
	int width = sourceImage.cols;
	int nChannels = sourceImage.channels();

	histMatrix = Mat(nChannels, 256, CV_32SC1, Scalar(0));
	uint32_t* pHistData = (uint32_t*)histMatrix.data;
	uint8_t* pImageData = (uint8_t*)sourceImage.data;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < nChannels; k++) {
				int index = i * width * nChannels + j * nChannels + k;
				pHistData[k * 256 + pImageData[index]]++;
			}
		}
	}*/
}

ColorTransformer::ColorTransformer()
{
}

ColorTransformer::~ColorTransformer()
{
}
