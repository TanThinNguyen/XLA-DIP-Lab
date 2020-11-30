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
	if (tx < 0 || ty < 0)
	{
		return 0;
	}

	// Tìm cận dưới l,k tương ứng của tx, ty
	int l = floor(tx);
	int k = floor(ty);

	float a = tx - l;
	float b = ty - k;

	// Nội suy giá trị màu f_new theo tỉ lệ với 4 lân cận
	float f_new = (1 - a) * (1 - b) * pSrc[srcWidthStep * k + nChannels * l]
		+ a * (1 - b) * pSrc[srcWidthStep * k + nChannels * (l + 1)]
		+ b * (1 - a) * pSrc[srcWidthStep * (k + 1) + nChannels * l]
		+ a * b * pSrc[srcWidthStep * (k + 1) + nChannels * (l + 1)];
	return uchar(round(f_new));
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

	// Tìm số nguyên x, y gần tx, ty nhất
	int x = int(round(tx));
	int y = int(round(ty));
	// Index của điểm ảnh mang giá trị màu nội suy cần tìm
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
	res_x = _matrixTransform.at<float>(0, 0) * x + _matrixTransform.at<float>(0, 1) * y + _matrixTransform.at<float>(0, 2);
	res_y = _matrixTransform.at<float>(1, 0) * x + _matrixTransform.at<float>(1, 1) * y + _matrixTransform.at<float>(1, 2);

	x = res_x;
	y = res_y;
}

AffineTransform::AffineTransform()
{
	// Khởi tạo ma trận đơn vị
	this->_matrixTransform = Mat::eye(3, 3, CV_32FC1);
}

AffineTransform::~AffineTransform()
{
}

int GeometricTransformer::Transform(const Mat& beforeImage, Mat& afterImage, AffineTransform* transformer, PixelInterpolate* interpolator)
{
	if (beforeImage.data == NULL || beforeImage.channels() != afterImage.channels())
	{
		return 0;
	}

	// Thông số ảnh afterImage
	int dstRow = afterImage.rows;
	int dstCol = afterImage.cols;
	int dstWidthStep = afterImage.step1();

	// Thông số ảnh beforeImage
	int srcRow = beforeImage.rows;
	int srcCol = beforeImage.cols;
	int srcWidthStep = beforeImage.step1();

	int nChannel = beforeImage.channels();

	uchar* pSrcData = (uchar*)beforeImage.data;
	uchar* pDstData = (uchar*)afterImage.data;

	// Duyệt trên ảnh afterImage
	for (int y = 0; y < dstRow; y++)
	{
		for (int x = 0; x < dstCol; x++)
		{
			// Tìm tọa độ tương ứng tx, ty ở ảnh beforeImage qua ánh xạ ngược
			float tx = x, ty = y;
			transformer->TransformPoint(tx, ty);

			// Duyệt từng kênh màu, tìm màu nội suy
			for (int k = 0; k < nChannel; k++)
			{
				uchar color = 0;
				if (tx >= 0 && tx <= srcCol && ty >= 0 && ty <= srcRow)
				{
					color = interpolator->Interpolate(tx, ty, pSrcData + k, srcWidthStep, nChannel);
				}
				
				// Gán màu nội suy cho điểm ảnh của afterImage
				int index = y * dstWidthStep + x * nChannel + k;
				pDstData[index] = color;
			}
		}
	}
	return 1;
}

int GeometricTransformer::RotateKeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
	{
		return 0;
	}

	// Thông số ảnh srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Tính toán dstRow, dstCol
	float w = float(srcCol);
	float h = float(srcRow);
	float diag = sqrt(w * w + h * h);

	float alpha0 = atan2(h, w);
	float new_w = max(fabs(diag * cos(-alpha0 + angle)),
					fabs(diag * cos(alpha0 + angle)));
	float new_h = max(fabs(diag * sin(-alpha0 + angle)),
					fabs(diag * sin(alpha0 + angle)));

	int dstRow = int(ceil(new_h));
	int dstCol = int(ceil(new_w));

	// Khởi tạo dstImage với kích thước thích hợp
	if (nChannel == 1)
		dstImage = Mat(dstRow, dstCol, CV_8UC1, Scalar(0));
	else if (nChannel == 3)
		dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// Tìm phép biến đổi affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Translate(-new_w / 2, -new_h / 2);
	affineTrans->Rotate(-angle);
	affineTrans->Translate(srcCol / 2.0f, srcRow / 2.0f);

	// Thực hiện biến đổi
	int ret = this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return ret;
}

int GeometricTransformer::RotateUnkeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
	{
		return 0;
	}

	// Thông số ảnh srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Tính toán dstRow, dstCol
	int dstRow = srcRow;
	int dstCol = srcCol;

	// Khởi tạo dstImage với kích thước thích hợp
	if (nChannel == 1)
		dstImage = Mat(dstRow, dstCol, CV_8UC1, Scalar(0));
	else if (nChannel == 3)
		dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// Tìm phép biến đổi affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Translate(-srcCol / 2.0f, -srcRow / 2.0f);
	affineTrans->Rotate(-angle);
	affineTrans->Translate(srcCol / 2.0f, srcRow / 2.0f);

	// Thực hiện biến đổi
	int ret = this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return ret;
}

int GeometricTransformer::Scale(const Mat& srcImage, Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL || sx < 0.001 || sy < 0.001)
	{
		return 0;
	}

	// Thông số ảnh srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Tính toán dstRow, dstCol
	int dstRow = int(ceil(srcRow * sy));
	int dstCol = int(ceil(srcCol * sx));

	// Khởi tạo dstImage với kích thước thích hợp
	dstImage = Mat(dstRow, dstCol, CV_8UC3, Scalar(0));

	// Tìm phép biến đổi affine ngược
	AffineTransform* affineTrans = new AffineTransform();
	affineTrans->Scale(1/sx, 1/sy);

	// Thực hiện biến đổi
	int ret = this->Transform(srcImage, dstImage, affineTrans, interpolator);
	return ret;
}

int GeometricTransformer::Resize(const Mat& srcImage, Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL || newWidth <= 0 || newHeight <= 0)
	{
		return 0;
	}

	// Thông số ảnh srcImage
	int srcRow = srcImage.rows;
	int srcCol = srcImage.cols;
	int nChannel = srcImage.channels();

	// Tính tỉ lệ scale
	float sx = float(newWidth) / srcCol;
	float sy = float(newHeight) / srcRow;

	// Thực hiện biến đổi thông qua phép Scale
	int ret = this->Scale(srcImage, dstImage, sx, sy, interpolator);
	return ret;
}

int GeometricTransformer::Flip(const Mat& srcImage, Mat& dstImage, bool direction, PixelInterpolate* interpolator)
{
	if (srcImage.data == NULL)
		return 0;

	int height = srcImage.rows;
	int width = srcImage.cols;
	int nChannels = srcImage.channels();

	//Khởi tạo dstImage bằng với chiều của srcImage
	dstImage = Mat(height, width, CV_8UC3, Scalar(0));

	//Lấy widthStep của srcImage và dstImage
	int dstWidthStep = dstImage.step1();
	int srcWidthStep = srcImage.step1();

	uchar* pSrcData = srcImage.data;
	uchar* pDstData = dstImage.data;

	//Trục ngang
	if (direction == 1) {
		//Lấy giá trị y ở giữa ảnh
		float axis_y = height / 2.0;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				//Giữ nguyên x
				float tx = x, ty;
				//Nếu y > axis_y thì y nằm ở trên trục, ngược lại y nằm dưới
				if (y >= axis_y)
					ty = y - axis_y;
				else
					ty = y + axis_y;

				// Duyệt từng kênh màu, tìm màu nội suy
				for (int k = 0; k < nChannels; k++)
				{
					uchar color = interpolator->Interpolate(tx, ty, pSrcData + k, srcWidthStep, nChannels);
					int index = y * dstWidthStep + x * nChannels + k;
					pDstData[index] = color;
				}
			}
		}
	}
	//Trục đứng
	else {
		//Lấy giá trị x ở giữa ảnh
		float axis_x = width / 2.0;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				//Giữ nguyên y
				float tx, ty = y;
				//Nếu x > axis_x thì y nằm ở bên phải trục, ngược lại y nằm bên trái trục
				if (x >= axis_x)
					tx = x - axis_x;
				else
					tx = x + axis_x;

				// Duyệt từng kênh màu, tìm màu nội suy
				for (int k = 0; k < nChannels; k++)
				{
					uchar color = interpolator->Interpolate(tx, ty, pSrcData + k, srcWidthStep, nChannels);
					int index = y * dstWidthStep + x * nChannels + k;
					pDstData[index] = color;
				}
			}
		}
	}

	return 1;
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
	//std::cout << result << std::endl;
	return result;
}
