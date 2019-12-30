#pragma once
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;
class Convolution
{
	vector<float> _kernel;// lưu giá trị của ma trận kernel dùng để tích chập
	int _kernelWidth;//chiều rộng kernel
	int _kernelHeight;//chiều dài kernel

public:
	//trả về kernel 
	vector<float> GetKernel()
	{
		return _kernel;
	}

	//set kernel, chiều rộng, chiều dài kernel phải là số lẻ
	void SetKernel(float kernel[], int kWidth, int kHeight)
	{
		if ((kWidth % 2) != 0 && (kHeight % 2)!= 0) return;
		for (int i = 0; i < kWidth * kHeight; i++)
			_kernel.push_back(kernel[i]);
	}

	/*
	Hàm tính convolution của 1 ảnh xám với kernel được xác định trước
	sourceImage: ảnh input
	destinationImage: ảnh output
	Hàm trả về
	0: nếu tính thành công
	1: nếu tính thất bại (không đọc được ảnh input,...)
	*/
	int DoConvolution(const Mat& sourceImage, Mat& destinationImage)
	{
		int nr = sourceImage.rows;
		int nc = sourceImage.cols;

		int kernelXIndex[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
		int kernelYIndex[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };

		// Tạo matrix để lưu giá trị pixel sau khi thực hiện tích chập.
		destinationImage.create(Size(nc, nr), CV_8UC1);

		// Đi lần lượt từng pixel của ảnh nguồn.
		for (int i = 0; i < nr; i++)
		{
			// Lấy địa chỉ dòng của ảnh đích, để lưu kết quả vào.
			uchar* data = destinationImage.ptr<uchar>(i);

			for (int j = 0; j < nc; j++)
			{
				// Lưu tổng giá trị độ xám của vùng ảnh tương ứng với kernel
				int val = 0;

				// Duyệt mask, giá trị pixel đích là tổ hợp tuyến tính của mask với ảnh nguồn.
				for (int ii = 0; ii < _kernel.size(); ii++)
				{
					//_kernelXIndex: mảng chỉ số truy cập nhanh cua dong
					int index_r = i - kernelXIndex[ii];

					// Với pixel nằm ngoài biên, bỏ qua.
					if (index_r < 0 || index_r > nr - 1)
						continue;

					//_kernelXIndex: mảng chỉ số truy cập nhanh cua cot
					int index_c = j - kernelYIndex[ii];

					// Với pixel nằm ngoài biên, bỏ qua.
					if (index_c < 0 || index_c > nc - 1)
						continue;
					// Cộng giá trị đã tính toán
					val += _kernel[ii] * sourceImage.at<uchar>(index_r, index_c);
				}

				// Gán giá trị cho matrix đích.
				data[j] = val;
			}
		}
		return 0;
	}

	//Convolution();
//	~Convolution();
};

