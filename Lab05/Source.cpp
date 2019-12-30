#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Threshold.h"
#include "Kmean.h"

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
	// So sánh câu lệnh và xử lý tương ứng theo lệnh nhập vào

	// Phân ngưỡng tĩnh ảnh
	if (cmdCompare(argv[1], "--static")) { // --static
		// Đọc ảnh đầu vào
		Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
		cout << argv[2] << " ";
		
		//Chuyển đổi ảnh sang định dạng Grayscale
		if (inputImage.type() == CV_8UC3)
			cvtColor(inputImage, inputImage, COLOR_RGB2GRAY);
		
		// Đọc giá trị ngưỡng trên và dưới
		int low = atoi(argv[3]);
		int up = atoi(argv[4]);
		StaticThreshold staticThres(low, up);

		// Khởi tạo ảnh đầu ra
		Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC1);
		
		staticThres.Apply(inputImage, outputImage);

		// Xuất ảnh đầu ra ra màn hình
		namedWindow("Show Image");
		imshow("Show Image", outputImage);
		waitKey(0);
	}
	else // Phân ngưỡng động theo trung bình
		if (cmdCompare(argv[1], "--mean")) { // --mean
			// Đọc ảnh đầu vào
			Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);

			//Chuyển đổi ảnh sang định dạng Grayscale
			if (inputImage.type() == CV_8UC3)
				cvtColor(inputImage, inputImage, COLOR_RGB2GRAY);

			// Đọc giá trị kích thước cửa sổ và hệ số C
			int winSize = atoi(argv[3]);
			int c = atoi(argv[4]);
			AverageLocalThreshold aveLocalThres(c);

			// Khởi tạo ảnh đầu ra
			Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC1);

			aveLocalThres.Apply(inputImage, outputImage, winSize);

			// Xuất ảnh đầu ra ra màn hình
			namedWindow("Show Image");
			imshow("Show Image", outputImage);
			waitKey(0);
		}
		else // Phân ngưỡng động theo trung vị
			if (cmdCompare(argv[1], "--median")) { // --rgb2hsv
				// Đọc ảnh đầu vào
				Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);

				//Chuyển đổi ảnh sang định dạng Grayscale
				if (inputImage.type() == CV_8UC3)
					cvtColor(inputImage, inputImage, COLOR_RGB2GRAY);

				// Đọc giá trị kích thước cửa sổ và hệ số C
				int winSize = atoi(argv[3]);
				int c = atoi(argv[4]);
				MedianLocalThreshold medLocalThres(c);

				// Khởi tạo ảnh đầu ra
				Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC1);

				medLocalThres.Apply(inputImage, outputImage, winSize);

				// Xuất ảnh đầu ra ra màn hình
				namedWindow("Show Image");
				imshow("Show Image", outputImage);
				waitKey(0);
			}
			else // Phân ngưỡng động Sauvola
				if (cmdCompare(argv[1], "--sauvola")) { // --sauvola
					// Đọc ảnh đầu vào
					Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);

					//Chuyển đổi ảnh sang định dạng Grayscale
					if (inputImage.type() == CV_8UC3)
						cvtColor(inputImage, inputImage, COLOR_RGB2GRAY);

					// Đọc giá trị kích thước cửa sổ và hệ số C
					int winSize = atoi(argv[3]);
					int r = atoi(argv[4]);
					float k = atof(argv[5]);
					SauvolaLocalThreshold sauvolaLocalThres(r, k);

					// Khởi tạo ảnh đầu ra
					Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC1);

					sauvolaLocalThres.Apply(inputImage, outputImage, winSize);

					// Xuất ảnh đầu ra ra màn hình
					namedWindow("Show Image");
					imshow("Show Image", outputImage);
					waitKey(0);
				}
				else // Phân đoạn theo K-means
					if (cmdCompare(argv[1], "--kmeans")) { // --sauvola
						// Đọc ảnh đầu vào
						Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);

						// Đọc giá trị số cụm 
						int k = atoi(argv[3]);

						Kmean kmean(k);

						// Khởi tạo ảnh đầu ra
						Mat outputImage = Mat(inputImage.rows, inputImage.cols, CV_8UC1);

						kmean.Apply(inputImage, outputImage);

						// Xuất ảnh đầu ra ra màn hình
						namedWindow("Show Image");
						imshow("Show Image", outputImage);
						waitKey(0);
					}


	system("pause");
	return 0;
}