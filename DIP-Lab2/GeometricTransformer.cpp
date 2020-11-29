#include "GeometricTransformer.h"

/* class PixelInterpolate */
PixelInterpolate::PixelInterpolate()
{
}

PixelInterpolate::~PixelInterpolate()
{
}

/* class BilinearInterpolate */
uchar BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels)
{
	return uchar();
}

BilinearInterpolate::BilinearInterpolate()
{
}

BilinearInterpolate::~BilinearInterpolate()
{
}

/* class NearestNeighborInterpolate */
uchar NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels)
{
	if (tx < 0 || ty < 0)
	{
		return 0;
	}

	int x = int(round(tx));
	int y = int(round(ty));

	int index = y * srcWidthStep + x * nChannels;
	return pSrc[index];
}

NearestNeighborInterpolate::NearestNeighborInterpolate()
{
}

NearestNeighborInterpolate::~NearestNeighborInterpolate()
{
}

void AffineTransform::Translate(float dx, float dy)
{
	Mat matrix = Mat::eye(3, 3, CV_32FC1);
	matrix.at<float>(0, 2) = dx;
	matrix.at<float>(1, 2) = dy;

	this->_matrixTransform = matrixDot(matrix, this->_matrixTransform);
}

void AffineTransform::Rotate(float angle)
{
	Mat matrix = Mat::zeros(3, 3, CV_32FC1);
	matrix.at<float>(0, 0) = cos(angle);
	matrix.at<float>(0, 1) = -sin(angle);
	matrix.at<float>(1, 0) = sin(angle);
	matrix.at<float>(1, 1) = cos(angle);
	matrix.at<float>(2, 2) = 1;

	this->_matrixTransform = matrixDot(matrix, this->_matrixTransform);
}

void AffineTransform::Scale(float sx, float sy)
{
	Mat matrix = Mat::zeros(3, 3, CV_32FC1);
	matrix.at<float>(0, 0) = sx;
	matrix.at<float>(1, 1) = sy;
	matrix.at<float>(2, 2) = 1;

	this->_matrixTransform = matrixDot(matrix, this->_matrixTransform);
}

void AffineTransform::TransformPoint(float& x, float& y)
{
	// Nhân vector [x, y, 1] với _matrixTransform
	float res_x = 0, res_y = 0;
	res_x = _matrixTransform.at<float>(0, 0) * x + _matrixTransform.at<float>(1, 0) * y + _matrixTransform.at<float>(2, 0);
	res_y = _matrixTransform.at<float>(0, 1) * x + _matrixTransform.at<float>(1, 1) * y + _matrixTransform.at<float>(2, 1);

	x = res_x;
	y = res_y;
}

AffineTransform::AffineTransform()
{
	// Khởi tạo ma trận đơn vị
	this->_matrixTransform = Mat(3, 3, CV_32FC1, Scalar(0));
	this->_matrixTransform.at<float>(0, 0) = 1;
	this->_matrixTransform.at<float>(1, 1) = 1;
	this->_matrixTransform.at<float>(2, 2) = 1;
}

AffineTransform::~AffineTransform()
{
}

int GeometricTransformer::Transform(const Mat& beforeImage, Mat& afterImage, AffineTransform* transformer, PixelInterpolate* interpolator)
{
	int dstRow = afterImage.rows;
	int dstCol = afterImage.cols;
	int dstWidthStep = afterImage.step1();
	int srcWidthStep = beforeImage.step1();
	int nChannel = beforeImage.channels();

	uchar* pSrcData = beforeImage.data;
	uchar* pDstData = afterImage.data;

	// Duyệt trên ảnh afterImage
	for (int y = 0; y <dstRow; y++)
	{
		for (int x = 0; x < dstCol; x++)
		{
			float tx = x, ty = y;

			// Tìm tọa độ tương ứng ở ảnh beforeImage qua ánh xạ ngược
			transformer->TransformPoint(tx, ty);

			// Duyệt từng kênh màu, tìm màu nội suy
			for (int k = 0; k < nChannel; k++)
			{
				uchar color = interpolator->Interpolate(tx, ty, pSrcData + k, srcWidthStep, nChannel);
				int index = y * dstWidthStep + x * nChannel + k;
				pDstData[index] = color;
			}
		}
	}
	return 0;
}

int GeometricTransformer::RotateKeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
	{
		return 0;
	}

	int rows = srcImage.rows;
	int cols = srcImage.cols;
	int nChannel = srcImage.channels();

	// Tính toán dstRow, dstCol

	// Khởi tạo dstImage với kích thước thích hợp

	// Tìm phép biến đổi affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Rotate(angle);

	// Thực hiện biến đổi
	this->Transform(srcImage, dstImage, affineTrans, interpolator);

	return 1;
}

int GeometricTransformer::RotateUnkeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	return 0;
}

int GeometricTransformer::Scale(const Mat& srcImage, Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
	{
		return 0;
	}

	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Tính toán dstRow, dstCol
	int dstRow = int(ceil(srcRow * sx));
	int dstCol = int(ceil(srcCol * sy));

	// Khởi tạo dstImage với kích thước thích hợp
	dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// Tìm phép biến đổi affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Scale(1/sx, 1/sy);

	// Thực hiện biến đổi
	this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return 1;
}

int GeometricTransformer::Resize(const Mat& srcImage, Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator)
{
	return 0;
}

int GeometricTransformer::Flip(const Mat& srcImage, Mat& dstImage, bool direction, PixelInterpolate* interpolator)
{
	return 0;
}

GeometricTransformer::GeometricTransformer()
{
}

GeometricTransformer::~GeometricTransformer()
{
}

Mat matrixDot(const Mat& mat1, const Mat& mat2)
{
	Mat result = Mat(3, 3, CV_32FC1, Scalar(0));
	
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float sum = 0;
			for (int k = 0; k < 3; k++)
			{
				sum += mat1.at<float>(i, k) * mat2.at<float>(k, j);
			}
			result.at<float>(i, j) = sum;
		}
	}

	return result;
}
