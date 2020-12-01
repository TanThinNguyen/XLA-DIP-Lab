#include "GeometricTransformer.h"

int main(int argc, char* argv[])
{
	/*
	Tham số dòng lệnh được đặt theo nguyên tắc:
	<Program.exe> <Command> <Interpolate> <InputPath> <CmdArguments>
	*/

	// Xác định phương pháp nội suy màu
	PixelInterpolate* interpolator;
	if (strcmp(argv[2], "--bl") == 0)
		interpolator = new BilinearInterpolate;
	else if (strcmp(argv[2], "--nn") == 0)
		interpolator = new NearestNeighborInterpolate;
	else
		return 0;

	Mat sourceImage = imread(argv[3], IMREAD_COLOR);
	//Mat sourceImage = imread(argv[3], IMREAD_GRAYSCALE);
	Mat destinationImage;
	GeometricTransformer* gmtTransform = new GeometricTransformer;

	/*
	Nếu dòng lệnh có 5 tham số thì có 3 trường hợp
	1 - Xoay ảnh quanh tâm bảo toàn nội dung
	2 - Xoay ảnh quanh tâm không bảo toàn nội dung
	3 - Đối xứng ảnh qua trục đứng hay trục ngang
	*/
	int check = -1;
	if (argc == 5)
	{
		// Xoay ảnh quanh tâm bảo toàn nội dung
		if (strcmp(argv[1], "--rotate") == 0)
		{
			// Người dùng nhập vào độ -> convert từ độ sang radian 
			float angle = atof(argv[4]) * PI / 180;
			check = gmtTransform->RotateKeepImage(sourceImage, destinationImage, angle, interpolator);
		}

		// Xoay ảnh quanh tâm bảo toàn nội dung
		else if (strcmp(argv[1], "--rotateN") == 0)
		{
			// Người dùng nhập vào độ -> convert từ độ sang radian 
			float angle = atof(argv[4]) * PI / 180;
			check = gmtTransform->RotateUnkeepImage(sourceImage, destinationImage, angle, interpolator);
		}

		// Đối xứng ảnh qua trục đứng hay trục ngang
		else if (strcmp(argv[1], "--flip") == 0)
		{
			// Trục đối xứng
			bool direction;
			if (strcmp(argv[4], "Ox") == 0)
				direction = 1;
			else if (strcmp(argv[4], "Oy") == 0)
				direction = 0;
			else
				return 0;

			check = gmtTransform->Flip(sourceImage, destinationImage, direction, interpolator);
		}
	}

	/*
	Nếu dòng lệnh có 6 tham số thì có 2 trường hợp
	1 - Phóng to, thu nhỏ ảnh
	2 - Thay đổi kích thước ảnh
	*/
	else if (argc == 6)
	{
		// Phóng to, thu nhỏ ảnh
		if (strcmp(argv[1], "--zoom") == 0)
		{
			check = gmtTransform->Scale(sourceImage, destinationImage, atof(argv[4]), atof(argv[5]), interpolator);
		}

		// Thay đổi kích thước ảnh
		else if (strcmp(argv[1], "--resize") == 0)
		{
			check = gmtTransform->Resize(sourceImage, destinationImage, atoi(argv[4]), atoi(argv[5]), interpolator);
		}
	}

	// Kiểm tra giá trị trả về của phép biến đổi
	if (check == 0)
		cout << "Bien doi that bai." << endl;
	else if (check == 1)
	{
		imwrite("destinationImage.jpg", destinationImage);
		imshow("Source Image", sourceImage);
		imshow("Destination Image", destinationImage);
	}

	waitKey(0);
	return 0;
}

/*
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
*/