#pragma once
#include "opencv2/core.hpp"
#include <opencv2/core/types_c.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <cmath>
#include <string>
#define PI 3.14159265
using namespace cv;
using namespace std;
/*
Lớp biểu diễn phép biến đổi affine
*/
class AffineTransform
{
private:
	Mat _matrixTransform; //ma trận 3x3 biểu diễn phép biến đổi affine
public:
	// xây dựng matrix transform cho phép tịnh tiến theo vector (dx,dy)
	void Translate(float dx, float dy)
	{
		/*  Tạo ma trận Affine 3x3 cho phép biến đổi Translate :[ 1 0 dx; 0 1 dy; 0 0 1 ]   */
		Mat _matrixTranslate = Mat::eye(3, 3, CV_32F);
		_matrixTranslate.at<float>(2, 0) = dx;
		_matrixTranslate.at<float>(2, 1) = dy;

		/*Nhân ma trận hiện hành cho ma trận biến đổi Affine Translate*/
		_matrixTransform = _matrixTransform * _matrixTranslate;
		_matrixTranslate.release();
	};

	//xây dựng matrix transform cho phép xoay 1 góc angle
	void Rotate(float angle) {
		/*  Tạo ma trận Affine 3x3 cho phép biến đổi Rotate :[ cos(angle) -sin(angle) 0; sin(angle) cos(angle) 0; 0 0 1 ]   */
		Mat _matrixRotate = Mat::eye(3, 3, CV_32F);
		_matrixRotate.at<float>(0, 0) = cos(angle * PI / 180);
		_matrixRotate.at<float>(0, 1) = -sin(angle * PI / 180);
		_matrixRotate.at<float>(1, 0) = sin(angle * PI / 180);
		_matrixRotate.at<float>(1, 1) = cos(angle * PI / 180);

		/*Nhân ma trận hiện hành cho ma trận biến đổi Affine Rotate*/
		_matrixTransform = _matrixTransform * _matrixRotate;
		_matrixRotate.release();
	};

	//xây dựng matrix transform cho phép tỉ lệ theo hệ số
	void Scale(float sx, float sy) {
		/*  Tạo ma trận Affine 3x3 cho phép biến đổi Scale :[ sx 0 0; 0 sy 0; 0 0 1 ]   */
		Mat _matrixScale = Mat::eye(3, 3, CV_32F);
		_matrixScale.at<float>(0, 0) = sx;
		_matrixScale.at<float>(1, 1) = sy;
		_matrixScale.at<float>(2, 2) = 1;

		/*Nhân ma trận hiện hành cho ma trận biến đổi Affine Scale */
		_matrixTransform = _matrixTransform * _matrixScale;
		_matrixScale.release();
	};

	//transform 1 điểm (x,y) theo matrix transform đã có
	void TransformPoint(float& x, float& y) {
		/*Tạo ma trận 3x1 cho tọa độ của điểm cũ [x; y; 1] */
		Mat _matrixPointOld = Mat::ones(3, 1, CV_32F);
		_matrixPointOld.at<float>(0, 0) = x;
		_matrixPointOld.at<float>(0, 1) = y;

		Mat _matrixPointNew;                                   // Ma trận để lưu kết quả sau khi áp dụng phép biến đổi Affine lên điểm P(x,y)
		_matrixPointNew = _matrixTransform * _matrixPointOld;  // Áp dụng phép biến đổi Affine hiện hành cho điểm P(x,y)
		x = _matrixPointNew.at<float>(0, 0);                   // Gía trị x mới =  _matrixPointNew[0][0]
		y = _matrixPointNew.at<float>(0, 1);                   // Gía trị y mới =  _matrixPointNew[0][1]
		_matrixPointOld.release();
		_matrixPointNew.release();
	};

	//lấy giá trị tại (x,y) của ma trận biến đổi
	float Get(int x, int y)
	{
		float result = _matrixTransform.at<float>(x, y);
		return result;
	};

	/*Hàm constructor của lớp Affine*/
	AffineTransform() {
		_matrixTransform = Mat::eye(3, 3, CV_32F); 	/* Khởi tạo ma trận cơ sở 3x3 :[ 1 0 0; 0 1 0; 0 0 1 ]   */
	};

	//Hàm distructor của lớp Affine
	~AffineTransform() {
		_matrixTransform.release();
	};
};
/*
 Lớp base dùng để nội suy màu của 1 pixel
*/
class PixelInterpolate
{
public:
	/*
	Hàm tính giá trị màu của ảnh kết quả từ nội suy màu trong ảnh gốc
	Tham số
		- (tx,ty): tọa độ thực của ảnh gốc sau khi thực hiện phép biến đổi affine
		- pSrc: con trỏ ảnh gốc
		- srcWidthStep: widthstep của ảnh gốc
		- nChannels: số kênh màu của ảnh gốc
	Trả về
		- Giá trị màu được nội suy
	*/
	virtual uchar Interpolate(float tx, float ty, Mat imgSrc, AffineTransform* Affine, int channel) = 0;
	PixelInterpolate() {};
	~PixelInterpolate() {};
};

/*
Lớp nội suy màu theo phương pháp song tuyến tính
*/
class BilinearInterpolate : public PixelInterpolate
{
public:
	uchar Interpolate(float tx, float ty, Mat imgSrc, AffineTransform* Affine, int channel){
		
		float a0, a1, a2, b0, b1, b2; //Lấy các giá trị trong ma trận Affine hiện hành ra [ a1 a2 a0; b1 b2 b0; 0 0 1]
		a0 = Affine->Get(2, 0);
		a1 = Affine->Get(0, 0);
		a2 = Affine->Get(1, 0);
		b0 = Affine->Get(2, 1);
		b1 = Affine->Get(0, 1);
		b2 = Affine->Get(1, 1);

		float x = ((ty - b0) * a2 - b2 * (tx - a0)) / (b1 * a2 - a1 * b2);// x, y là nghiệm của hệ pt bậc nhất 1 ẩn: a1*x + a2*y=tx-a0
		float y = ((ty - b0) * a1 - b1 * (tx - a0)) / (b2 * a1 - a2 * b1);//                                         b1*x + b2*y=ty-b0
		int l = floor(x);                                                 // Làm tròn giá trị của x
		int k = floor(y);                                                 // Làm tròn giá trị của y
		float a = x - l;                                                  // tính độ chênh lệch giữa giá trị thực và giá trị làm tròn của x
		float b = y - k;                                                  // tính độ chênh lệch giữa giá trị thực và giá trị làm tròn của y
		uchar Q_00 = 0, Q_10 = 0, Q_01 = 0, Q_11 = 0;                     // Khởi tạo giá trị tại 4 pixel lân cận = 0
		if (k >= 0 && k < imgSrc.rows && l >= 0 && l < imgSrc.cols) {
			// thay thế Q_00 bằng giá trị màu tại điểm(l,k)
			if (imgSrc.channels() == 1) {
				Q_00 = imgSrc.at<uchar>(k, l);                            
			}
			else {
				Q_00 = imgSrc.at<Vec3b>(k, l)[channel];
			}
		}
		if (k >= 0 && k < imgSrc.rows && l + 1 >= 0 && l + 1 < imgSrc.cols) {
			// thay thế Q_10 bằng giá trị màu tại điểm(l+1,k)
			if (imgSrc.channels() == 1) {
				Q_10 = imgSrc.at<uchar>(k, l + 1);
			}
			else {
				Q_10 = imgSrc.at<Vec3b>(k, l + 1)[channel];
			}
		}

		if (k + 1 >= 0 && k + 1 < imgSrc.rows && l >= 0 && l < imgSrc.cols) {
			// thay thế Q_01 bằng giá trị màu tại điểm(l,k+1)
			if (imgSrc.channels() == 1) {
				Q_01 = imgSrc.at<uchar>(k + 1, l);
			}
			else {
				Q_01 = imgSrc.at<Vec3b>(k + 1, l)[channel];
			}
		}
		if (k + 1 >= 0 && k + 1 < imgSrc.rows && l + 1 >= 0 && l + 1 < imgSrc.cols) {
			// thay thế Q_11 bằng giá trị màu tại điểm(l+1,k+1)
			if (imgSrc.channels() == 1) {
				Q_11 = imgSrc.at<uchar>(k + 1, l + 1);
			}
			else {
				Q_11 = imgSrc.at<Vec3b>(k + 1, l + 1)[channel];
			}
		}
		
		uchar result = round((1 - a) * (1 - b) * Q_00 + a * (1 - b) * Q_10 + b * (1 - a) * Q_01 + a * b * Q_11);  // Tính kết quả thông qua biểu thức nội suy tuyến tính  
		                                                                                                          // f(x,y) = (1-a)(1-b)f(l,k) + a(1-b)f(l+1,k) + b(1-a)f(l,k+1)+abf(l+1,k+1)
		return result;
};
	BilinearInterpolate() {};
	~BilinearInterpolate() {};
};

/*
Lớp nội suy màu theo phương pháp láng giềng gần nhất
*/
class NearestNeighborInterpolate : public PixelInterpolate
{
public:
	uchar Interpolate(float tx, float ty, Mat imgSrc, AffineTransform* Affine, int channel = 1) {		
		float a0, a1, a2, b0, b1, b2; //Lấy các giá trị trong ma trận Affine hiện hành ra [ a1 a2 a0; b1 b2 b0; 0 0 1]
		a0 = Affine->Get(2, 0);
		a1 = Affine->Get(0, 0);
		a2 = Affine->Get(1, 0);
		b0 = Affine->Get(2, 1);
		b1 = Affine->Get(0, 1);
		b2 = Affine->Get(1, 1);

		int x = round(((ty - b0) * a2 - b2 * (tx - a0)) / (b1 * a2 - a1 * b2));// x, y là nghiệm của hệ pt bậc nhất 1 ẩn: a1*x + a2*y=tx-a0
		int y = round(((ty - b0) * a1 - b1 * (tx - a0)) / (b2 * a1 - a2 * b1));//                                         b1*x + b2*y=ty-b0
		                                                                       // sau đó làm tròn x và y
		if (x < 0 || x >= imgSrc.cols || y < 0 || y >= imgSrc.rows) // kiểm tra xem điểm đó có ở ngoài hỉnh ảnh ko. Nếu có thì trả về giá trị màu là 0 (màu đen)
		{
			return 0;
		}
		if (imgSrc.channels() == 1) // Nếu kênh màu là 1. trả về giá trị của màu tại vị trí (x,y)
		{
			return imgSrc.at<uchar>(y, x);
		}
		
		return imgSrc.at<Vec3b>(y, x)[channel]; // Nếu kênh màu là 3 trả về giá trị màu tại vị trí (x,y) và kênh màu channel
	};
	NearestNeighborInterpolate() {};
	~NearestNeighborInterpolate() {};
};



/*
Lớp thực hiện phép biến đổi hình học trên ảnh
*/

class GeometricTransformer
{
public:
	/*
	Hàm biến đổi ảnh theo 1 phép biến đổi affine đã có
	Tham số
	 - beforeImage: ảnh gốc trước khi transform
	 - afterImage: ảnh sau khi thực hiện phép biến đổi affine
	 - transformer: phép biến đổi affine
	 - interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Transform(
		const Mat& beforeImage,
		Mat& afterImage,
		AffineTransform* transformer,
		PixelInterpolate* interpolator) {

		if (beforeImage.empty()) // Kiểm tra xem ảnh có được mở không
		{
			return 0; 
		}

		afterImage = Mat(round(beforeImage.rows* transformer->Get(1,1)), round(beforeImage.cols * transformer->Get(0, 0)), beforeImage.type()); // tạo ảnh mới có kích thước mới dựa vào ma trận Affine hiện tại		
		float xX, yY;
		// duyệt từng pixel 1 trên ảnh kết quả
		for (int j = 0; j < afterImage.rows; j++) {
			for (int i = 0; i < afterImage.cols; i++) {
				xX = (float)i;
				yY = (float)j;
				if (beforeImage.channels() == 1) // Nếu kênh màu = 1
				{
					afterImage.at<uchar>(j, i) = interpolator->Interpolate(xX, yY, beforeImage, transformer, 0);//Nội suy ảnh tại vị trí (i, j)
				}
				else // Nếu kênh màu = 3
				{
					afterImage.at<Vec3b>(j, i)[0] = interpolator->Interpolate(xX, yY, beforeImage, transformer, 0); //Nội suy ảnh tại vị trí(i,j) kênh số 0
					afterImage.at<Vec3b>(j, i)[1] = interpolator->Interpolate(xX, yY, beforeImage, transformer, 1);//Nội suy ảnh tại vị trí(i,j) kênh số 1
					afterImage.at<Vec3b>(j, i)[2] = interpolator->Interpolate(xX, yY, beforeImage, transformer, 2);//Nội suy ảnh tại vị trí(i,j) kênh số 2
				}				
			}
		}
		return 1;
	};

	/*
	Hàm xoay bảo toàn nội dung ảnh theo góc xoay cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay
	- angle: góc xoay (đơn vị: độ)
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int RotateKeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator) {
		if (srcImage.empty()) // Kiểm tra xem ảnh có được mở không
		{
			return 0;
		}
		int newW = round(srcImage.cols * abs(cos(angle * PI / 180.0)) + srcImage.rows * abs(sin(angle * PI / 180.0))); //Chiều rộng mới của ảnh = chiều rộng cũ *cos(angle) + chiều cao cũ* (sin anle)
		int newH = round(srcImage.rows * abs(cos(angle * PI / 180.0)) + srcImage.cols * abs(sin(angle * PI / 180.0)));//Chiều cao mới của ảnh = chiều rộng cũ *cos(angle) + chiều cao cũ* (sin anle)
		dstImage = Mat(newH, newW, srcImage.type()); // Tạo ảnh mới theo kích thước mới

		AffineTransform x; //Khai báo biến AffineTransform 
		  // dịch chuyển tâm từ ảnh cũ về tâm theo ảnh mới
		x.Translate((newW / 2) -(srcImage.cols/2) , (newH / 2) - (srcImage.rows / 2));
		x.Translate(-newW/2, -newH/2 );                                                // đưa tâm mới về gốc tọa độ
		x.Rotate(angle);                                                                  // Xoay ảnh quanh gốc tọa độ
		x.Translate(newW/2, newH/2);                                                  // Đưa tâm ảnh về vị trí cũ
		float xX, yY;

		// Duyệt từng pixel trên ảnh mới
		for (int j = 0; j < dstImage.rows; j++) {
			for (int i = 0; i < dstImage.cols; i++) {
				xX = (float)i;
				yY = (float)j;
				if (srcImage.channels() == 1) // Nếu kênh màu = 1
				{
					dstImage.at<uchar>(j, i) = interpolator->Interpolate(xX, yY, srcImage, &x, 0);//Nội suy ảnh tại vị trí (i, j)
				}
				else //Nếu số kênh màu là 3
				{
					dstImage.at<Vec3b>(j, i)[0] = interpolator->Interpolate(xX, yY, srcImage, &x, 0);//Nội suy ảnh tại vị trí(i,j) kênh số 0
					dstImage.at<Vec3b>(j, i)[1] = interpolator->Interpolate(xX, yY, srcImage, &x, 1);//Nội suy ảnh tại vị trí(i,j) kênh số 1
					dstImage.at<Vec3b>(j, i)[2] = interpolator->Interpolate(xX, yY, srcImage, &x, 2);//Nội suy ảnh tại vị trí(i,j) kênh số 2
				}
			}
		}
		return 1;
	};

	/*
	Hàm xoay không bảo toàn nội dung ảnh theo góc xoay cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay
	- angle: góc xoay (đơn vị: độ)
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int RotateUnkeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator) {
		if (srcImage.empty()) // Kiểm tra xem ảnh có được mở không
		{
			return 0;
		}
		dstImage = Mat(round(srcImage.rows), round(srcImage.cols), srcImage.type()) ; // Tạo ảnh mới có kích thước bằng kích thước ảnh cũ
		AffineTransform x; //Khai báo biến AffineTransform
		x.Translate(-srcImage.cols / 2, - srcImage.rows / 2);// Dịch chuyển tâm xoay về gốc tọa độ
		x.Rotate(angle);                                     // Xoay ảnh quanh gốc tọa độ
		x.Translate(srcImage.cols / 2, srcImage.rows / 2);   // Dịch chuyển tâm về vị trí cũ
		float xX, yY;
		// Duyệt từng pixel của ảnh mới
		for (int j = 0; j < dstImage.rows; j++) {
			for (int i = 0; i < dstImage.cols; i++) {
				xX = (float)i;
				yY = (float)j;
				if (srcImage.channels() == 1)  // Nếu kênh màu =1
				{
					dstImage.at<uchar>(j, i) = interpolator->Interpolate(xX, yY, srcImage, &x, 0); //Nội suy ảnh tại vị trí (i, j)
				}
				else  // Nếu số kênh màu = 3
				{
					dstImage.at<Vec3b>(j, i)[0] = interpolator->Interpolate(xX, yY, srcImage, &x, 0);//Nội suy ảnh tại vị trí(i,j) kênh số 0
					dstImage.at<Vec3b>(j, i)[1] = interpolator->Interpolate(xX, yY, srcImage, &x, 1);//Nội suy ảnh tại vị trí(i,j) kênh số 1
					dstImage.at<Vec3b>(j, i)[2] = interpolator->Interpolate(xX, yY, srcImage, &x, 2);//Nội suy ảnh tại vị trí(i,j) kênh số 2
				}
			}
		}
		return 1;
	};

	/*
	Hàm phóng to, thu nhỏ ảnh theo tỉ lệ cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay
	- sx, sy: tỉ lệ phóng to, thu nhỏ ảnh
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Scale(
		const Mat& srcImage,
		Mat& dstImage,
		float sx, float sy,
		PixelInterpolate* interpolator)
	{
		if (srcImage.empty())  // Kiểm tra xem ảnh có được mở không
		{
			return 0;
		}
		dstImage = Mat(round(srcImage.rows * sy), round(srcImage.cols * sx), srcImage.type()); // Tạo ảnh mới có kích thước mới: width = oldWidth*sx, height= oldHeight*sy
		AffineTransform x; //Khai báo biến AffineTransform
		x.Scale(sx, sy);   //gọi hàm Scale(sx,sy)
		float xX, yY;
		//duyệt từng pixel của ảnh mới
		for (int j = 0; j < dstImage.rows; j++) {
			for (int i = 0; i < dstImage.cols; i++) {
				xX = (float)i;
				yY = (float)j;
				if (srcImage.channels() == 1) // Nếu kênh màu =1
				{
					dstImage.at<uchar>(j, i) = interpolator->Interpolate(xX, yY, srcImage, &x, 0); //Nội suy ảnh tại vị trí (i, j)
				}
				else {
					dstImage.at<Vec3b>(j, i)[0] = interpolator->Interpolate(xX, yY, srcImage, &x, 0);//Nội suy ảnh tại vị trí(i,j) kênh số 0
					dstImage.at<Vec3b>(j, i)[1] = interpolator->Interpolate(xX, yY, srcImage, &x, 1);//Nội suy ảnh tại vị trí(i,j) kênh số 1
					dstImage.at<Vec3b>(j, i)[2] = interpolator->Interpolate(xX, yY, srcImage, &x, 2);//Nội suy ảnh tại vị trí(i,j) kênh số 2
				}
			}
		}
		return 1;
	};


	/*
	Hàm thay đổi kích thước ảnh
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thay đổi kích thước
	- newWidth, newHeight: kích thước mới
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Resize(
		const Mat& srcImage,
		Mat& dstImage,
		int newWidth, int newHeight,
		PixelInterpolate* interpolator) {
		float sx, sy;
		sx = (float) newWidth / srcImage.cols;                          //Tính tỉ lệ (sx) giữa chiều rộng mới so với chiều rộng cũ
		sy = (float) newHeight / srcImage.rows;                         //Tính tỉ lệ (sy) giữa chiều cao mới so với chiều cao cũ
		return this->Scale(srcImage, dstImage, sx, sy, interpolator);   //Gọi đến hàm scale theo tỉ lệ (sx,sy)
	};

	/*
	Hàm lấy đối xứng ảnh
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi lấy đối xứng
	- direction = 1 nếu lấy đối xứng theo trục ngang và direction = 0 nếu lấy đối xứng theo trục đứng
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Flip(
		const Mat& srcImage,
		Mat& dstImage,
		bool direction,
		PixelInterpolate* interpolator) {
		
		if (srcImage.empty()) // Kiểm tra xem ảnh có được mở không
		{
			return 0;
		}
		dstImage = Mat(round(srcImage.rows), round(srcImage.cols ), srcImage.type()); //tạo ảnh mới có kích thước bằng ảnh cũ
		AffineTransform x;
		if (!direction) // Nếu direction == 0
		{
			x.Translate(-srcImage.cols,0); // Dịch ảnh về sau trục Oy
			x.Scale(-1, 1);                // Scale(-1,1) để ảnh đối xứng qua trục Oy;
		}
		else  // Nếu direction == 1
		{
			x.Translate(0, -srcImage.rows); // Dịch ảnh về sau trục Ox
			x.Scale(1, -1);                  // Scale(1,-1) để ảnh đối xứng qua trục Ox;
		}
		float xX, yY;
		//Duyệt từng pixel trên ảnh mới
		for (int j = 0; j < dstImage.rows; j++) {
			for (int i = 0; i < dstImage.cols; i++) {
				xX = (float)i;
				yY = (float)j;
				if (srcImage.channels() == 1) //Nếu số kênh màu ==1
				{
					dstImage.at<uchar>(j, i) = interpolator->Interpolate(xX, yY, srcImage, &x, 0); //Nội suy ảnh tại vị trí (i, j)
				}
				else //Nếu số kênh màu ==3
				{
					dstImage.at<Vec3b>(j, i)[0] = interpolator->Interpolate(xX, yY, srcImage, &x, 0);//Nội suy ảnh tại vị trí(i,j) kênh số 0
					dstImage.at<Vec3b>(j, i)[1] = interpolator->Interpolate(xX, yY, srcImage, &x, 1);//Nội suy ảnh tại vị trí(i,j) kênh số 1
					dstImage.at<Vec3b>(j, i)[2] = interpolator->Interpolate(xX, yY, srcImage, &x, 2);//Nội suy ảnh tại vị trí(i,j) kênh số 2
				}
			}
		}
		return 1;
	};

	GeometricTransformer() {};
	~GeometricTransformer() {};
};

