#pragma once

#include "opencv2/core.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/core/types_c.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <cmath>
#include <string>
#define PI 3.14159265
using namespace cv;
using namespace std;

class CannyEdgeDetector
{
	//ngưỡng dưới
	int _lowThreshold;
	//ngưỡng trên
	int _highThreshold;

public:
	/*
		Hàm áp dụng thuật toán Canny để phát hiện biên cạnh
		- srcImage: ảnh input
		- dstImage: ảnh kết quả
		Hàm trả về
			1: nếu detect thành công
			0: nếu detect không thành công
	*/
	void grayToScale(const Mat& srcimage, Mat& dstImage) {
		cvtColor(srcimage, dstImage, COLOR_BGR2GRAY);    //Chuyển đổi ảnh màu thành ảnh Gray
	}

	int Apply(const Mat& srcImage, Mat& dstImage) {

		Mat detected_edges;                              //Khai báo ma trận ảnh để xử lý;
		Mat srcGray;
		int Gx, Gy;

		if (srcImage.empty()) {                                      // Kiểm tra xem ảnh đã được đọc hay chưa
			cout << "Could not open or find the image!\n" << endl;
			return 0;
		}

		grayToScale(srcImage, srcGray);                       // Chuyển ảnh màu về ảnh xám
		GaussianBlur(srcGray, detected_edges, Size(3, 3), 1, 1, BORDER_DEFAULT); //Giảm nhiễu ảnh gốc

		dstImage.create(detected_edges.size(), detected_edges.type());  // Tạo ảnh đích có cùng kích thước và kiểu so với ảnh đã giảm nhiễu
		dstImage = Scalar::all(0);                          // lấp toàn bộ ảnh đích bằng màu đen

		int** temp = new int* [detected_edges.cols]; // chứa toàn bộ giá trị ảnh đã chỉnh sửa;
		for (int i = 0; i < detected_edges.cols; i++)
		{
			temp[i] = new int[detected_edges.rows];
		}
		for (int i = 0; i < detected_edges.cols; i++)
			for (int j = 0; j < detected_edges.rows; j++)
			{
				temp[i][j] = detected_edges.at<uchar>(j, i);
			}


		int GX[3][3] = //Khai báo ma trận Sobel GX
		{
		{ -1, 0, 1 },
		{ -2, 0, 2 },
		{ -1, 0, 1 }
		};

		int GY[3][3] =      //Khai báo ma trận Sobel GY
		{
		{ -1, -2, -1 },
		{ 0, 0, 0 },
		{ 1, 2, 1 }
		};

		int** GradientMagnitude = new int* [detected_edges.cols];              //Khai báo ma trận cường độ gradient
		for (int i = 0; i < detected_edges.cols; i++)
		{
			GradientMagnitude[i] = new int[detected_edges.rows];
		}

		int** EdgeDirection = new int* [detected_edges.cols];                //Khai báo ma trận hướng ảnh
		for (int i = 0; i < detected_edges.cols; i++)
		{
			EdgeDirection[i] = new int[detected_edges.rows];
		}

		int** NonMaskSuppress = new int* [detected_edges.cols];        //Khai báo mặt nạ nhị phân
		for (int i = 0; i < detected_edges.cols; i++)
		{
			NonMaskSuppress[i] = new int[detected_edges.rows];
		}

		
		for (int i = 1; i < detected_edges.cols - 1; i++)
			for (int j = 1; j < detected_edges.cols - 1; j++)
			{
				/*Tính cường độ theo ma trận Sobel*/
				Gx = temp[i - 1][j - 1] * GX[0][0] + temp[i - 1][j] * GX[0][1] + temp[i - 1][j + 1] * GX[0][2]       //Tính ảnh đạo hàm Gx
					+ temp[i][j - 1] * GX[1][0] + temp[i][j] * GX[1][1] + temp[i][j + 1] * GX[1][2]
					+ temp[i + 1][j - 1] * GX[2][0] + temp[i + 1][j] * GX[2][1] + temp[i + 1][j + 1] * GX[2][2];
				Gy = temp[i - 1][j - 1] * GY[0][0] + temp[i - 1][j] * GY[0][1] + temp[i - 1][j + 1] * GY[0][2]       //Tính ảnh đạo hàm Gy
					+ temp[i][j - 1] * GY[1][0] + temp[i][j] * GY[1][1] + temp[i][j + 1] * GY[1][2]
					+ temp[i + 1][j - 1] * GY[2][0] + temp[i + 1][j] * GY[2][1] + temp[i + 1][j + 1] * GY[2][2];

				GradientMagnitude[i][j] = (int)(sqrt((double)(Gx * Gx + Gy * Gy)));                                  //Tính độ lớn của biến đổi mức sáng tại vị trí (i,j) tương ứng trên ảnh gốc

				float thisAngle = float((atan2((float)Gx, (float)Gy) / 3.14) * 180.0);   //Tính góc (hướng của cạnh)

				/*Convert actual edge direction to approximate value*/
				int newAngle;
				if (((thisAngle < 22.5) && (thisAngle > -22.5)) || (thisAngle > 157.5) || (thisAngle < -157.5))            //Làm tròn về hướng ngang (0 độ)
					newAngle = 0;
				else if (((thisAngle > 22.5) && (thisAngle < 67.5)) || (thisAngle < -112.5) || (thisAngle > -157.5))       //Làm tròn về hướng chéo bên phải (45 độ)
					newAngle = 45;
				else if (((thisAngle > 67.5) && (thisAngle < 112.5)) || (thisAngle < -67.5) || (thisAngle > -112.5))       //Làm tròn về hướng dọc (90 độ)
					newAngle = 90;
				else if (((thisAngle > 112.5) && (thisAngle < 157.5)) || (thisAngle < -22.5) || (thisAngle > -67.5))       //Làm tròn về hướng chéo bên trái (135 độ)
					newAngle = 135;

				EdgeDirection[i][j] = newAngle; // Store the approximate edge direction of each pixel in one array         //Lưu hướng làm tròn vào  ma trận hướng ảnh EdgeDirection
			}
		/*Non maximum suppresion - Loại bỏ các pixel ở vị trí không phải cực đại toàn cục*/
		for (int i = 1; i < detected_edges.cols - 1; i++)
			for (int j = 1; j < detected_edges.rows - 1; j++)
			{
				switch (EdgeDirection[i][j])
				{
				case 0:                                     //Hướng 0 độ
					if ((GradientMagnitude[i][j] >= GradientMagnitude[i - 1][j]) && (GradientMagnitude[i][j] >= GradientMagnitude[i + 1][j]))              //Nếu GradientMagnitude: (i,j) >= [(i-1,j) && (i+1,j)] (Nghĩa là cực đại lân cận) 
						NonMaskSuppress[i][j] = GradientMagnitude[i][j];                                                                                   // giữ lại pixel (i,j)
					else
						NonMaskSuppress[i][j] = 0;   //Nếu không phải cực đại thì set pixel về 0
					break;
				case 45:
					if ((GradientMagnitude[i][j] >= GradientMagnitude[i - 1][j - 1]) && (GradientMagnitude[i][j] >= GradientMagnitude[i + 1][j + 1]))
						NonMaskSuppress[i][j] = GradientMagnitude[i][j];
					else
						NonMaskSuppress[i][j] = 0;
					break;
				case 90:
					if ((GradientMagnitude[i][j] >= GradientMagnitude[i][j - 1]) && (GradientMagnitude[i][j] >= GradientMagnitude[i][j + 1]))
						NonMaskSuppress[i][j] = GradientMagnitude[i][j];
					else
						NonMaskSuppress[i][j] = 0;
					break;
				case 135:
					if ((GradientMagnitude[i][j] >= GradientMagnitude[i - 1][j + 1]) && (GradientMagnitude[i][j] >= GradientMagnitude[i + 1][j - 1]))
						NonMaskSuppress[i][j] = GradientMagnitude[i][j];
					else
						NonMaskSuppress[i][j] = 0;
					break;
				default:
					break;
				}
			}

		
	/*	int max = 0, min = 0;
		for (int i = 2; i < detected_edges.cols - 2; i++)
			for (int j = 2; j < detected_edges.rows - 2; j++)
			{
				if (NonMaskSuppress[i][j] > max)
					max = NonMaskSuppress[i][j];
				else if (NonMaskSuppress[i][j] < min)
					min = NonMaskSuppress[i][j];
			}
		int temporary;
		for (int i = 1; i < detected_edges.cols - 1; i++)
			for (int j = 1; j < detected_edges.rows - 1; j++)
			{
				if (NonMaskSuppress[i][j] > 0)
					temporary = (NonMaskSuppress[i][j] - min) * 255 / (max - min);
				else temporary = 0;
				NonMaskSuppress[i][j] = temporary;
			}*/
			//Lọc ngưỡng
			// Nếu giá trị gradient vượt quá ngưỡng max_val thì pixel đó chắc chắn là cạnh
			// Nếu gradient đó nhỏ hơn ngưỡng min_val thì bỏ qua pixel đó
		for (int i = 1; i < detected_edges.cols - 1; i++)
			for (int j = 1; j < detected_edges.rows - 1; j++)
			{
				if (NonMaskSuppress[i][j] > this->_highThreshold)
					temp[i][j] = 255;
				else if (NonMaskSuppress[i][j] < this->_lowThreshold) 
					temp[i][j] = 0;
				else
					switch (EdgeDirection[i][j])
					{
					case 0:
						if ((NonMaskSuppress[i - 1][j] > this->_highThreshold) || (NonMaskSuppress[i - 1][j] > this->_highThreshold))
							temp[i][j] = 255;
						else
							temp[i][j] = 0;
						break;
					case 45:
						if ((NonMaskSuppress[i - 1][j - 1] > this->_highThreshold) || (NonMaskSuppress[i + 1][j + 1] > this->_highThreshold))
							temp[i][j] = 255;
						else
							temp[i][j] = 0;
						break;
					case 90:
						if ((NonMaskSuppress[i][j - 1] > this->_highThreshold) || (NonMaskSuppress[i][j + 1] > this->_highThreshold))
							temp[i][j] = 255;
						else
							temp[i][j] = 0;
						break;
					case 135:
						if ((NonMaskSuppress[i - 1][j + 1] > this->_highThreshold) || (NonMaskSuppress[i + 1][j - 1] > this->_highThreshold))
							temp[i][j] = 255;
						else
							temp[i][j] = 0;
						break;
					default:
						break;
					}
			}
		for (int i = 0; i < detected_edges.cols; i++)
			for (int j = 0; j < detected_edges.cols; j++)
			{
				dstImage.at<uchar>(j, i) = temp[i][j];
			}
		for (int i = 0; i < detected_edges.cols; i++)
		{
			delete[] temp[i];
			delete[] GradientMagnitude[i];
			delete[] EdgeDirection[i];
			delete[] NonMaskSuppress[i];
		}
		delete[] temp;
		delete[] GradientMagnitude;
		delete[] EdgeDirection;
		delete[] NonMaskSuppress;
		detected_edges.release();
		srcGray.release();
		return 1;
	}

	CannyEdgeDetector() {
		this->_highThreshold = 30;
		this->_lowThreshold = 0;
	};
	CannyEdgeDetector(int lowThre, int heightThre) {
		this->_highThreshold = heightThre;
		this->_lowThreshold = lowThre;
	};
	~CannyEdgeDetector() {};
};

