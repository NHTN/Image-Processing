#pragma once
#include <math.h>
#include <iostream>
#include <ctime>
#include "Kmean.h"
#define INF 10e9


// Khởi tạo Cluster từ dữ liệu ảnh đầu vào
vector<Cluster> Kmean::initClusters(const Mat& srcImage) {
		// vector clusters chứa kết quả trả về
		vector<Cluster> clusters;
		// Khởi tạo giá trị random theo time
		srand(time(NULL));

		// Lấy giá trị kích thước ảnh
		int row = srcImage.rows;
		int col = srcImage.cols;

		// Khởi tạo theo số cụm cluster đã khai báo
		for (int indexCluster = 0; indexCluster < _numClusters; indexCluster++) {
			Cluster k;
			// Đối với ảnh 1 kênh màu
			if (srcImage.channels() == 1) {
				// Gán giá trị tại pixel random trong miền kích thước ảnh đầu vào cho cluster
				k.value = srcImage.at<uchar>(rand() % (row - 1), rand() % (col - 1));
			}
			else // Đối với ảnh 3 kênh màu
			{
				// Gán giá trị tại pixel random trong miền kích thước ảnh đầu vào cho cluster
				k.value3 = srcImage.at<Vec3b>(rand() % (row - 1), rand() % (col - 1));
			}
			// Gán nhãn cho cluster
			k.label = indexCluster;

			// Thêm cluster thứ index vào vector
			clusters.push_back(k);
		}

		// Trả về vector cluster đã được khởi tạo
		return clusters;
	}


	// Hàm tính khoảng khác biệt giữa 2 pixel 3 kênh màu bằng công thức Euclid
	int EuclideanDistance(Vec3b pixel1, Vec3b pixel2) {
		return (int)sqrt(pow(pixel1[0] - pixel2[0], 2) + pow(pixel1[1] - pixel2[1], 2) + pow(pixel1[2] - pixel2[2], 2));
	}

	// Hàm tính khoảng khác biệt giữa 2 pixel 1 kênh màu
	uchar diffGrayScale(uchar a, uchar b) {
		uchar diff = a - b;
		return (diff < 0) ? -diff : diff;
	}

	//Lấy giá trị trung bình của cluster đối với ảnh có 1 kênh màu
	uchar meanCluster(const Mat& srcImage, int** pixels, int label) {
		vector<uchar> temp;

		// Lấy kích thước của ảnh đầu vào
		int rows = srcImage.rows;
		int cols = srcImage.cols;

		// Duyệt qua ảnh đầu vào 
		for (int row = 0; row < srcImage.rows; row++) {
			for (int col = 0; col < srcImage.cols; col++) {

				if (pixels[row][col] == label) {
					temp.push_back(srcImage.at<uchar>(row, col));
				}
			}

		}

		// Tính tổng giá trị của vector temp
		long int sum = 0;
		for (int k = 0; k < temp.size(); k++) {
			sum += temp[k];
		}

		// Trả về giá trị trung bình của vector temp
		if (temp.size() < 1)
			return 255;
		else
			return (sum / temp.size());
	}

	// Lấy giá trị trung bình của cluster đối với ảnh có 3 kênh màu
	Vec3b meanCluster3(const Mat& srcImage, int** pixels, int label) {
		vector<Vec3b> temp;

		// Lấy kích thước của ảnh
		int rows = srcImage.rows;
		int cols = srcImage.cols;

		// Duyệt qua các pixel của ảnh
		for (int row = 0; row < srcImage.rows; row++) {
			for (int col = 0; col < srcImage.cols; col++) {
				// Thêm giá trị vào vector temp nếu pixel tại vị trí row, col = giá trị nhãn dán 
				if (pixels[row][col] == label) {
					temp.push_back(srcImage.at<Vec3b>(row, col));
				}
			}

		}

		// Tính tổng từng kênh màu
		long int sumR = 0, sumG = 0, sumB = 0;

		for (int k = 0; k < temp.size(); k++) {
			sumR += temp[k][0];
			sumG += temp[k][1];
			sumB += temp[k][2];
		}
		if (temp.size() < 1)
			return 255;

		// Trả về giá trị trung bình 3 kênh màu
		Vec3b mean = Vec3b(sumR / temp.size(), sumG / temp.size(), sumB / temp.size());

		return mean;
	}

	int Kmean::Apply(const Mat& srcImage, Mat& dstImage) {
		//if (srcImage.type() != CV_8UC1) {
			//return 0;
		//}

		// Khởi tạo ảnh output với kích thước của ảnh input, số kênh màu là 1 
		dstImage = srcImage.clone();

		// Khởi tạo vector Cluster
		vector <Cluster> clusters = initClusters(srcImage);

		// Lấy kích thước của ảnh
		int rows = srcImage.rows;
		int cols = srcImage.cols;


		// Khởi tạo các mảng chứa pixel rỗng theo kích thước của ảnh đầu vào
		int** pixels = new int* [rows];

		//Khởi tạo mảng pixel với giá trị -1
		for (int row = 0; row < rows; row++) {
			pixels[row] = new int[cols];
			for (int col = 0; col < cols; col++) {
				pixels[row][col] = -1;
			}
		}


		// Khởi tạo giá trị index vòng lặp và vector cluster lưu trạng thái trước của cluster
		int index = 0;
		vector <Cluster> prevs;

		//
		while (true) {
			prevs = clusters;

			// Duyệt qua từng pixel của ảnh
			for (int row = 0; row < rows; row++) {
				for (int col = 0; col < cols; col++) {
					// Khởi tạo minDiff lưu giá trị khoảng khác biệt nhỏ nhất
					int minDiff = 0;
					// Tính khoảng khác biệt nhỏ nhất giữa pixel đang xét và các cluster
					
					// Xét với giá trị cluster[0] để khởi tạo minDiff
					if (srcImage.channels() == 1) {
						minDiff = diffGrayScale(srcImage.at<uchar>(row, col), clusters[0].value);
					}
					else {
						minDiff = EuclideanDistance(srcImage.at<Vec3b>(row, col), clusters[0].value3);
					}

					int diff = 0;
					int label = 0;

					// Xét với các cluster còn lại
					// Nếu nhỏ hơn giá trị minDiff thì lưu lại giá trị và gán nhãn 
					for (int k = 1; k < clusters.size(); k++) {
						if (srcImage.channels() == 1) {
							diff = diffGrayScale(srcImage.at<uchar>(row, col), clusters[k].value);
						}
						else {
							diff = EuclideanDistance(srcImage.at<Vec3b>(row, col), clusters[k].value3);
						}
						if (diff < minDiff) {
							minDiff = diff;
							label = k;
						}
					}

					// Gán giá trị nhãn dán của cluster chứa minDiff với pixel đang xét
					pixels[row][col] = clusters[label].label;

				}

			}

			// Gán giá trị trung bình cho cluster thứ k
			for (int k = 0; k < clusters.size(); k++) {
				if (srcImage.channels() == 1) {
					clusters[k].value = meanCluster(srcImage, pixels, clusters[k].label);
				}
				else {
					clusters[k].value3 = meanCluster3(srcImage, pixels, clusters[k].label);
				}
			}

			int cambiati = 0;

			// So sánh độ tương đồng 
			for (int k = 0; k < clusters.size(); k++) {
				if (prevs[k].value != clusters[k].value) {
					cambiati++;
				}
			}

			// Nếu tương đồng, dừng vòng lặp
			if (cambiati == 0) {
				break;
			}

			// Index lưu giá trị biến đếm vòng lặp
			index++;
		}


		// Gán giá trị cho ảnh đầu ra
		for (int row = 0; row < srcImage.rows; row++) {
			for (int col = 0; col < srcImage.cols; col++) {
				if (srcImage.channels() == 1) {
					dstImage.at<uchar>(row, col) = clusters[pixels[row][col]].value;
				}
				else {
					dstImage.at<Vec3b>(row, col) = clusters[pixels[row][col]].value3;
				}
			}
		}

		delete[] pixels;
		// Chuyển đổi ảnh thành công
		return 1;
	};

	Kmean::Kmean(int k) {
		_numClusters = k;
	};

	Kmean::~Kmean() {
	};


