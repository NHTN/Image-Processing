#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>

using namespace std;
using namespace cv;

// Khởi tạo struct Cluster chứa các trường dữ liệu như sau 
struct Cluster {
	uchar value;   // Giá trị màu nếu ảnh có 1 kênh màu
	Vec3b value3;	// Giá trị màu nếu ảnh có 3 kênh màu
	int label;		// Nhãn dán của cluster
};

class Kmean
{
	//số cụm K
	int _numClusters;

public:
	/*
	Hàm áp dụng thuật toán Kmeans để phân đoạn ảnh
	- srcImage: ảnh input
	- dstImage: ảnh kết quả
	Hàm trả về
	1: nếu phân đoạn thành công
	0: nếu phân đoạn không thành công
	*/
	vector<Cluster> initClusters(const Mat& srcImage);

	int Apply(const Mat& srcImage, Mat& dstImage);

	Kmean(int k);
	~Kmean();
};

