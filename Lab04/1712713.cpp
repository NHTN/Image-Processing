// 1712713.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "CannyEdgeDetector.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
using namespace std;
using namespace cv;

int lowThreshold = 0, highThreshold = 25;
const int ratio = 3;

int main(int argc, char* argv[]) {
	string Command = argv[1];                                //Lưu tên Command từ console	
	int val;                                                 // Khai báo để lưu các giá trị lỗi hoặc thành công được trả 
	string InputPath = argv[4];                              //Lưu địa chỉ File ảnh cần mở
	string OutputPath = argv[5];                              //Lưu địa chỉ File ảnh kết quả
	Mat inputImage = imread(InputPath, IMREAD_COLOR);          // Đọc ảnh 3 kênh màu
	Mat outputImage;                                         // Khai báo biến để lưu ảnh kết quả
	if (Command.compare("--canny") == 0)                      // Nếu Command=="--canny"
	{
	
		string StrLowThreshold = argv[2];                               //Lưu tên lowThreshold
		string StrHeightThreshold = argv[3];                            //Lưu tên HeightThreshold

		CannyEdgeDetector Canny(stoi(StrLowThreshold), stoi(StrHeightThreshold));    // Khai báo lớp GeometricTransformer để thực hiện các phép biến đổi
		val = Canny.Apply(inputImage, outputImage);
	}else
		val = -1;
	switch (val) {
	case 0:                 // Nếu val==0. Xuất ra thông báo không đọc được file hoặc không thực hiện được phép biến đổi
		cout << "Khong mo duoc file hoac khong thuc hien duoc phep bien doi" << endl;
		break;
	case 1:                 // Nếu val==1. Phép biến đổi thành công
		cout << "Anh duoc bien doi thanh cong" << endl;
		imwrite(OutputPath, outputImage);         //Xuất ảnh kết quả
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
