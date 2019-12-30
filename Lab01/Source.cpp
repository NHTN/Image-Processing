#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ColorTransformer.h"
#include "Converter.h"
#include <string.h>
#include <iomanip>

using namespace cv;
using namespace std;

bool cmdCompare(char* cmd, string type) {
	for (int len = 0; len < type.size(); len++) {
		if (cmd[len] != type[len]) 
			return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
	ColorTransformer transfer;
	Converter convert;
	// So sánh câu lệnh và xử lý tương ứng theo lệnh nhập vào

	// Chuyển đổi ảnh hệ RGB sang ảnh Grayscale
  	if (cmdCompare(argv[1], "--rgb2gray")) { // --rgb2gray
		// Đọc ảnh đầu vào
		Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR); 

		// Khởi tạo ảnh đầu ra
		Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC1); 

		// Gọi hàm xử lý ảnh - chuyển đổi hệ rgb sang grayscale
		convert.Convert(inputImage, outputImage, 0); 
		
		// Xuất ảnh đầu ra ra màn hình
		namedWindow("Show Image"); 
		imshow("Show Image", outputImage);
		waitKey(0);
	}
	else
		if (cmdCompare(argv[1], "--gray2rgb")) { // --gray2rgb
			Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
			Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC3);
			convert.Convert(inputImage, outputImage, 1);
			namedWindow("Show Image");
			imshow("Show Image", outputImage);
			waitKey(0);
		}
		else 
			if (cmdCompare(argv[1], "--rgb2hsv")) { // --rgb2hsv
				Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
				Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC3);
				convert.Convert(inputImage, outputImage, 2);
				namedWindow("Show Image");
				imshow("Show Image", outputImage);
				waitKey(0);
			} 
			else 
				if (cmdCompare(argv[1], "--hsv2rgb")) { // Chuyển đổi ảnh hệ HSV sang hệ RGB
					Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
					Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC3);
					convert.Convert(inputImage, outputImage, 3);
					namedWindow("Show Image");
					imshow("Show Image", outputImage);
					waitKey(0);
				} 
				else
					if (cmdCompare(argv[1], "--bright")) { // Tăng giảm độ sáng của ảnh
						Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
						Mat outputImage = inputImage.clone();
						transfer.ChangeBrighness(inputImage, outputImage, atoi(argv[3]));
						namedWindow("Show Image");
						imshow("Show Image", outputImage);
						waitKey(0);
					} 
					else 
						if (cmdCompare(argv[1], "--contrast")) { // Tăng giảm độ tương phản của ảnh
							Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
							Mat outputImage = inputImage.clone();
							transfer.ChangeContrast(inputImage, outputImage, atof(argv[3]));
							namedWindow("Show Image");
							imshow("Show Image", outputImage);
							waitKey(0);
						} 
						else 
							if (cmdCompare(argv[1], "--hist")) { // Tính histogram của ảnh
								Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
								float histMatrix[256];
								transfer.CalcHistogram(inputImage, histMatrix);
								for (int i = 0; i < 255; i++) {
									cout << fixed << setprecision(6) << histMatrix[i] << "\t";
								}
								cout << endl;
								waitKey(0);
							} 
							else 
								if (cmdCompare(argv[1], "--equalhist")) { // Cân bằng histogram
									Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
									Mat outputImage = inputImage.clone();

									transfer.HistogramEqualization(inputImage, outputImage);

									namedWindow("Show Image");
									imshow("Show Image", outputImage);
									waitKey(0);
								}
								else 
									if (cmdCompare(argv[1], "--drawhist")) { // Vẽ histogram của ảnh
										Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
										transfer.DrawHistogram(inputImage);
										waitKey(0);
									}
									else 
										if (cmdCompare(argv[1], "--compare")) { // So sánh 2 ảnh
											Mat image1 = imread(argv[2], IMREAD_ANYCOLOR);
											Mat image2 = imread(argv[3], IMREAD_ANYCOLOR);
											printf("%f\n", transfer.CompareImage(image1, image2)); 
										}
	system("pause");
	return 0;
}