#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include "Convolution.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

float tempSobelKernel1[9] = { -1, 0, 1,
							 -2, 0, 2,
							 -1, 0, 1 };

float tempSobelKernel2[9] = { -1,-2,-1,
							0, 0, 0,
							1, 2, 1 };

float tempPrewittKernel1[9] = { 5, 5, 5,
							  -3, 0,-3,
							  -3,-3,-3 };

float tempPrewittKernel2[9] = { 5, -3, -3,
							  5, 0, -3,
							  5, -3, -3 };

float tempLaplaceKernel[9] = { 0, 1, 0,
							   1, -4, 1,
							   0, 1, 0 };

class EdgeDetector
{
public:
	/*
	Hàm phát hiện biên cạnh của ảnh xám
	sourceImage: ảnh input
	destinationImage: ảnh output
	method: phương pháp detect
	kWidth, kHeight: kích thước kernel	
		1: Sobel
		2: Prewitt
		3: Laplace		
	Hàm trả về
		0: nếu detect thành công
		1: nếu detect thất bại (không đọc được ảnh input,...)
	*/

	int Sobel(const Mat& sourceImage, Mat& destinationImage)
	{
		int nr = sourceImage.rows;
		int nc = sourceImage.cols;
		Convolution SobelKernel;
		Mat Img1, Img2;

		//Nap mang Sobel 1
		SobelKernel.SetKernel(tempSobelKernel1, 3, 3);

		SobelKernel.DoConvolution(sourceImage, Img1);
		//Nap mang Sobel 2
		SobelKernel.SetKernel(tempSobelKernel2, 3, 3);

		SobelKernel.DoConvolution(sourceImage, Img2);

		//Tạo matrix để lưu giá trị pixel
		destinationImage.create(Size(nc, nr), CV_8UC1);

		for (int i = 0; i < nr; i++)
		{
			// Lấy địa chỉ dòng của ảnh đích, để lưu kết quả vào.
			uchar* data = destinationImage.ptr<uchar>(i);
			// Lay dia chi dong cua Img1
			uchar* data1 = Img1.ptr<uchar>(i);
			//Lay dia chi dong cua Img2
			uchar* data2 = Img2.ptr<uchar>(i);

			//gan tong gia tri tuyet doi cua hai mang vua tich chap vao anh dich 
			for (int j = 0; j < nc; j++)
			{
				int val = 0;
				
				if (data1[j] >= 0) val += data1[j];
				else val += data1[j];

				if (data2[j] >= 0) val += data2[j];
				else val += data2[j];

				// Gán giá trị cho matrix đích.
				data[j] = val;
			}
		}
		
		return 0;
	}

	int Prewitt(const Mat& sourceImage, Mat& destinationImage)
	{
		int nr = sourceImage.rows;
		int nc = sourceImage.cols;
		Convolution PrewittKernel;
		Mat Img1, Img2;

		//Nap mang Prewitt 1
		PrewittKernel.SetKernel(tempPrewittKernel1, 3, 3);

		PrewittKernel.DoConvolution(sourceImage, Img1);
		//Nap mang Prewitt 2
		PrewittKernel.SetKernel(tempPrewittKernel2, 3, 3);

		PrewittKernel.DoConvolution(sourceImage, Img2);

		// Tạo matrix để lưu giá trị pixel
		destinationImage.create(Size(nc, nr), CV_8UC1);

		for (int i = 0; i < nr; i++)
		{
			// Lấy địa chỉ dòng của ảnh đích, để lưu kết quả vào.
			uchar* data = destinationImage.ptr<uchar>(i);
			// Lay dia chi dong cua Img1
			uchar* data1 = Img1.ptr<uchar>(i);
			//Lay dia chi dong cua Img2
			uchar* data2 = Img2.ptr<uchar>(i);

			//gan tong gia tri tuyet doi cua hai mang vua tich chap vao anh dich 
			for (int j = 0; j < nc; j++)
			{
				int val = 0;

				if (data1[j] > 0) val += data1[j];
				else val += -data1[j];

				if (data2[j] > 0) val += data2[j];
				else val += -data2[j];

				// Gán giá trị cho matrix đích.
				data[j] = val;
			}
		}

		return 0;
	}

	int Laplace(const Mat& sourceImage, Mat& destinationImage)
	{
		int nr = sourceImage.rows;
		int nc = sourceImage.cols;
		Convolution LaplaceKernel;

		//Nap mang Prewitt 1
		LaplaceKernel.SetKernel(tempLaplaceKernel, 3, 3);

		LaplaceKernel.DoConvolution(sourceImage, destinationImage);
		return 0;
	}

	int DetectEdge(const Mat& sourceImage, Mat& destinationImage, int method)
	{
		if (sourceImage.ptr(0) != NULL)
		{
			EdgeDetector func;
			if (method == 1) func.Sobel(sourceImage, destinationImage);
			else if (method == 2) func.Prewitt(sourceImage, destinationImage);
			else if (method == 3) func.Laplace(sourceImage, destinationImage);
			else return 1;
			return 0;
		}
		return 1;
	}
	
	//EdgeDetector();
	//~EdgeDetector();
};

