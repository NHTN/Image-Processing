#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>

using namespace std;
using namespace cv;


//phân ngưỡng tĩnh
class StaticThreshold
{
	//ngưỡng dưới
	int _lowThreshold = 10;
	//ngưỡng trên
	int _highThreshold = 50;
public:
	/*
	Hàm áp dụng phân ngưỡng tĩnh
	- srcImage: ảnh input
	- dstImage: ảnh kết quả	
	Hàm trả về
	1: nếu phân ngưỡng thành công
	0: nếu phân ngưỡng không thành công
	*/

	int Apply(const Mat& srcImage, Mat& dstImage);

	StaticThreshold(int low, int high);
	~StaticThreshold();
};


//phân ngưỡng cục bộ dựa vào trung bình
class AverageLocalThreshold
{
	//hệ số C
	int _C;

public:
	/*
	Hàm áp dụng phân ngưỡng cục bộ theo trung bình
	- srcImage: ảnh input
	- dstImage: ảnh kết quả
	- winSize: kích thước lân cận
	Hàm trả về
		1: nếu phân ngưỡng thành công
		0: nếu phân ngưỡng không thành công
	*/

	int Apply(const Mat& srcImage, Mat& dstImage, int winSize);

	AverageLocalThreshold(int c);
	~AverageLocalThreshold();
};

//phân ngưỡng cục bộ dựa vào trung vị
class MedianLocalThreshold
{
	//hệ số C
	int _C = 0;

public:
	/*
	Hàm áp dụng phân ngưỡng cục bộ dựa vào trung vị
	- srcImage: ảnh input
	- dstImage: ảnh kết quả
	- winSize: kích thước lân cận
	Hàm trả về
	1: nếu phân ngưỡng thành công
	0: nếu phân ngưỡng không thành công
	*/

	int Apply(const Mat& srcImage, Mat& dstImage, int winSize);

	MedianLocalThreshold(int c);
	~MedianLocalThreshold();
};

//phân ngưỡng cục bộ dựa vào thuật toán Sauvola
class SauvolaLocalThreshold
{
	//hệ số r
	int _r = 128;
	//hệ số k
	float _k = 0.5;
public:
	/*
	Hàm áp dụng thuật toán Sauvola để phân ngưỡng
	- srcImage: ảnh input
	- dstImage: ảnh kết quả
	- winSize: kích thước lân cận
	Hàm trả về
	1: nếu phân ngưỡng thành công
	0: nếu phân ngưỡng không thành công
	*/

	int Apply(const Mat& srcImage, Mat& dstImage, int winSize);

	SauvolaLocalThreshold(int r, float k);
	~SauvolaLocalThreshold();
};
