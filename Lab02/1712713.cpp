// 1712713.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Transformer.h"
#include <iostream>
#include <stdio.h>
using namespace std;
using namespace cv;
int main(int argc, char* argv[]) {
	string Command = argv[1];                                //Lưu tên Command từ console
	string Interpolate = argv[2];                            //Lưu tên Interpolate từ console
	string InputPath = argv[3];                              //Lưu địa chỉ File ảnh cần mở
	int val;                                                 // Khai báo để lưu các giá trị lỗi hoặc thành công được trả về
	GeometricTransformer Geo;                                // Khai báo lớp GeometricTransformer để thực hiện các phép biến đổi
	Mat inputImage = imread(argv[3], IMREAD_COLOR);          // Đọc ảnh 3 kênh màu
	Mat outputImage = Mat();                                 // Khai báo biến để lưu ảnh kết quả

	if (Command.compare("--zoom") == 0)                      // Nếu Command=="--zoom"
	{
		string sx = argv[4];                                 //Lấy tham số sx
		string sy = argv[5];                                 //Lấy tham số sy

		if (Interpolate.compare("--bl") == 0)                //Nếu Interpolate=="--bl"
		{
			BilinearInterpolate bl;                          //Khai báo lớp BilinearInterpolate
			val = Geo.Scale(inputImage, outputImage, stof(sx), stof(sy), &bl);            // Thực hiện zoom theo tỉ lệ (sx,sy)
		}
		else if (Interpolate.compare("--nn") == 0)             //Nếu Interpolate=="--nn"
		{
			NearestNeighborInterpolate nn;                     //Khai báo lớp NearestNeighborInterpolate
			val = Geo.Scale(inputImage, outputImage, stof(sx), stof(sy), &nn);            // Thực hiện zoom theo tỉ lệ (sx,sy)
		}
		else val = -1;                                         // Ngược lại trả về lỗi không đúng lệnh
	}
	else if (Command.compare("--resize") == 0)                  // Nếu Command=="--resize"
	{
		string width = argv[4];                                //Lấy tham số width
		string height = argv[5];                               //Lấy tham số height

		if (Interpolate.compare("--bl") == 0)                  //Nếu Interpolate=="--bl"
		{
			BilinearInterpolate bl;
			val = Geo.Resize(inputImage, outputImage, stoi(width), stoi(height), &bl);  // Thực hiện resize theo kích thước mới (widht,height)
		}
		else if (Interpolate.compare("--nn") == 0)              //Nếu Interpolate=="--nn"
		{
			NearestNeighborInterpolate nn;
			val = Geo.Resize(inputImage, outputImage, stoi(width), stoi(height), &nn);  // Thực hiện resize theo kích thước mới (widht,height)
		}
		else val = -1;
	}
	else if (Command.compare("--rotate") == 0)                   // Nếu Command=="--rotate"
	{
		string angle = argv[4];                                   //Lấy tham số angle
		if (Interpolate.compare("--bl") == 0)                      //Nếu Interpolate=="--bl"
		{
			BilinearInterpolate bl;
			val = Geo.RotateKeepImage(inputImage, outputImage, stof(angle), &bl);    // Thực hiện RotateKeepImage theo góc xoay angle
		}
		else if (Interpolate.compare("--nn") == 0)                //Nếu Interpolate=="--nn"
		{
			NearestNeighborInterpolate nn;
			val = Geo.RotateKeepImage(inputImage, outputImage, stof(angle), &nn);    // Thực hiện RotateKeepImage theo góc xoay angle
		}
		else val = -1;
	}
	else if (Command.compare("--rotateN") == 0)                      // Nếu Command=="--rotateN"
	{
		string angle = argv[4];                                //Lấy tham số angle
		if (Interpolate.compare("--bl") == 0)
		{
			BilinearInterpolate bl;                             //Nếu Interpolate=="--bl"
			val = Geo.RotateUnkeepImage(inputImage, outputImage, stof(angle), &bl);    // Thực hiện RotateUnKeepImage theo góc xoay angle
		}
		else if (Interpolate.compare("--nn") == 0)              //Nếu Interpolate=="--nn"
		{
			NearestNeighborInterpolate nn;
			val = Geo.RotateUnkeepImage(inputImage, outputImage, stof(angle), &nn);   // Thực hiện RotateUnKeepImage theo góc xoay angle
		}
		else val = -1;
	}
	else if (Command.compare("--flip") == 0)                   // Nếu Command=="--flip"
	{
		string direction = argv[4];                            //Lấy tham số direction

		if (Interpolate.compare("--bl") == 0)                  //Nếu Interpolate=="--bl"
		{
			BilinearInterpolate bl;
			val = Geo.Flip(inputImage, outputImage, stoi(direction), &bl);            // Thực hiện Flip theo direction
		}
		else if (Interpolate.compare("--nn") == 0)              //Nếu Interpolate=="--nn"
		{
			NearestNeighborInterpolate nn;
			val = Geo.RotateUnkeepImage(inputImage, outputImage, stoi(direction), &nn);  // Thực hiện Flip theo direction
		}
		else val = -1;
	}
	else   //Nếu Command không thuộc các trường hợp trên. Trả về -1
	{
		val = -1;
	}
	switch (val) {
	case 0:                 // Nếu val==0. Xuất ra thông báo không đọc được file hoặc không thực hiện được phép biến đổi
		cout << "Khong mo duoc file hoac khong thuc hien duoc phep bien doi" << endl;
		break;
	case 1:                 // Nếu val==1. Phép biến đổi thành công
		cout << "Anh duoc bien doi thanh cong" << endl;
		namedWindow("Show Source Image");         //Tạo cửa sổ mới để hiển thị ảnh cũ
		imshow("Show Source Image", inputImage);  //Hiển thị ảnh cũ
		namedWindow("Show Dist Image");           //Tạo cửa sổ mới để hiển thị ảnh mới
		imshow("Show Dist Image", outputImage);   //Hiển thị ảnh mới
		waitKey(0);
		inputImage.release();                     //giải phóng ma trận ảnh cũ
		outputImage.release();                    //giải phóng ma trận ảnh mới
		break;
	case -1:             //Nếu val=-1. Trả về lỗi không đúng cú pháp
		cout << "Khong dung cau lenh. Vui long kiem tra lai cu phap" << endl;
		break;
	
	default:
		break;
	}
	return 0;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
