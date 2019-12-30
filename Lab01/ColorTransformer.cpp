#pragma once
#include "ColorTransformer.h"


ColorTransformer::ColorTransformer() {
};

ColorTransformer::~ColorTransformer() {
};


/*
Hàm nhận vào một ảnh, thay đổi độ sáng của ảnh này và lưu kết quả vào ảnh mới
Tham so:
	sourceImage	: ảnh ban đầu
	destinationImage: ảnh kết quả
	b	: giá trị số nguyên dùng để thay đổi độ sáng của ảnh
Hàm trả về:
	1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
	0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
*/
int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b) {
	// Sao chép ảnh đích từ ảnh input
	destinationImage = sourceImage.clone();
	// Lấy số kênh màu của ảnh
	int nChannels = sourceImage.channels();

	// Duyệt qua tất cả các pixel của ảnh
	for (int indexRow = 0; indexRow < sourceImage.rows; indexRow++) {
		for (int indexCol = 0; indexCol < sourceImage.cols; indexCol++) {
			for (int channel = 0; channel < nChannels; channel++) {
				// Trường hợp ảnh 3 kênh màu R, G, B
				if (nChannels == 3) {
					destinationImage.at<Vec3b>(indexRow, indexCol)[channel] = saturate_cast<uchar>
						(sourceImage.at<Vec3b>(indexRow, indexCol)[channel] + b);
				}
				else {
					// Trường hợp ảnh 1 kênh màu GrayScale
					destinationImage.at<uchar>(indexRow, indexCol) = saturate_cast<uchar>
						(sourceImage.at<uchar>(indexRow, indexCol) + b);
				}
			}
		}
	}
	return 1;
};

/*
Hàm nhận vào một ảnh, thay đổi độ tương phản của ảnh này và lưu kết quả vào ảnh mới
Tham so :
	sourceImage : ảnh ban đầu
	destinationImage : ảnh kết quả
	c	: giá trị số thực dùng để thay đổi độ tương phản của ảnh
Hàm trả về:
	1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
	0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
*/
int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c) {
	// Sao chép ảnh đích từ ảnh input
	destinationImage = sourceImage.clone();
	// Lấy số kênh màu của ảnh đầu vào
	int nChannels = sourceImage.channels();

	// Duyệt qua tất cả các pixel của ảnh
	for (int indexRow = 0; indexRow < sourceImage.rows; indexRow++) {
		for (int indexCol = 0; indexCol < sourceImage.cols; indexCol++) {
			for (int channel = 0; channel < nChannels; channel++) {
				// Trường hợp ảnh 3 kênh màu R, G, B
				if (nChannels == 3) {
					destinationImage.at<Vec3b>(indexRow, indexCol)[channel] = saturate_cast<uchar>
						(c * (destinationImage.at<Vec3b>(indexRow, indexCol)[channel]));
				}
				else {
					// Trường hợp ảnh 1 kênh màu GrayScale
					destinationImage.at<uchar>(indexRow, indexCol) = saturate_cast<uchar>
						(sourceImage.at<uchar>(indexRow, indexCol) * c);
				}
			}
		}
	}

	// Điều chỉnh độ tương phản thành công
	return 1;
};

/*
Hàm tạo mảng tính histogram của ảnh
Tham số:
	image: Ảnh cần tính histogram
	histogram: Mảng số thực lưu giá trị histogram của ảnh
	size:
*/
void calHistogram(Mat image, float histogram[], int size) {
	// Khởi tạo mảng histogram với giá trị 0
	for (int i = 0; i < 256; i++) {
		histogram[i] = 0;
	}

	// Duyệt qua tất cả các pixel của ảnh cần tính histogram
	for (int row = 0; row < image.rows; row++) {
		for (int col = 0; col < image.cols; col++) {
			// Tăng giá trị tại vị trí điểm ảnh trên mảng histogram
			histogram[(int)image.at<uchar>(row, col)]++;
		}
	}
	// Chia các phần tử trong mảng histogram cho kích thước của ảnh
	for (int index = 0; index < 256; index++) {
		histogram[index] = histogram[index] / size;
	}
}


/*
	Hàm tính lược đồ xám tích lũy của histogram
	Tham số đầu vào:
		- Mảng histogram
		- Mảng lưu giá trị lược đồ xám tích lũy cần tính
*/
void calTransferFunction(float histogram[], float transferFunc[]) {
	transferFunc[0] = histogram[0];
	for (int i = 1; i < 256; i++) {
		transferFunc[i] = histogram[i] + transferFunc[i - 1];
	}
}

/*
	Hàm tính lược đồ xám dựa trên mảng giá trị Intensity của ảnh HSI
	Tham số đầu vào:
		- Mảng intensity lưu giá trị Intensity của ảnh HSI đầu vào
		- Mảng histogram lưu giá trị lược đồ xám của ảnh đầu vào
		- Số nguyên rows thể hiện số hàng của ảnh
		- Số nguyên cols thể hiện số cột của ảnh
*/
void intensity2Histogram(float** intensity, float histogram[], int rows, int cols) {
	// Khởi tạo giá trị mảng lược đồ xám bằng 0
	for (int i = 0; i < 256; i++)
		histogram[i] = 0;

	// 
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			histogram[(int)intensity[row][col]]++;
		}
	}

	// Chuẩn hóa mảng về đoạn [0, rows*cols]
	for (int i = 0; i < 255; i++) {
		histogram[i] = histogram[i] / (rows * cols);
	}
}


void intensityMapping(float transferFunc[], float histogram[]) {
	float tranFuncMin = 256;

	for (int i = 0; i < 256; i++) {
		if (tranFuncMin > transferFunc[i]) {
			tranFuncMin = transferFunc[i];
		}
	}

	for (int i = 0; i < 256; i++) {
		histogram[i] = (((transferFunc[i] - tranFuncMin) / (1 - tranFuncMin)) * (256 - 1) + 0.5);
	}
}


/*
Hàm vẽ lược đồ histogram dựa của ảnh
Tham số đầu vào: Ảnh cần được vẽ lược đồ

*/
void ColorTransformer::DrawHistogram(Mat& image) {
	int bins = 256;             // number of bins
	int nChannels = image.channels();    // Số lượng kênh màu của ảnh
	vector<Mat> histogram(nChannels);    // Vector lưu trữ giá trị histogram của ảnh
	vector<Mat> canvas(nChannels);       // Ảnh biểu diễn cho lược đồ histogram
	int hmax[3] = { 0,0,0 };      // Giá trị max của từng histogram

	// Khởi tạo mảng histogram với giá trị 0 
	for (int i = 0; i < histogram.size(); i++)
		histogram[i] = Mat::zeros(1, bins, CV_32SC1);

	// Tính giá trị tần số xuất hiện điểm ảnh 
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			for (int k = 0; k < nChannels; k++) {
				uchar val = ((nChannels == 1) ? image.at<uchar>(i, j) : image.at<Vec3b>(i, j)[k]);
				histogram[k].at<int>(val)++;
			}
		}
	}

	// Duyệt tìm hmax cho mỗi histogram
	for (int i = 0; i < nChannels; i++) {
		for (int j = 0; j < bins - 1; j++)
			hmax[i] = ((histogram[i].at<int>(j) > hmax[i]) ? histogram[i].at<int>(j) : hmax[i]);
	}

	// Khởi tạo giá trị màu đỏ, xanh dương, xanh lá
	Scalar colors[3] = { Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255) };

	// Duyệt qua các kênh màu của ảnh - Có bao nhiêu kênh màu thì vẽ bấy nhiêu đồ thị histogram tương ứng
	for (int i = 0; i < nChannels; i++) {
		// Khởi tạo ma trận histogram cần vẽ
		canvas[i] = Mat::ones(125, bins, CV_8UC3);

		// Vẽ đồ thị 
		// Nếu 1 kênh màu thì vẽ đồ thị trắng đen,
		// Nếu 3 kênh màu thì vẽ 3 đồ thị tương ứng với 3 màu đỏ - xanh dương - xanh lá
		for (int j = 0, rows = canvas[i].rows; j < bins - 1; j++) {
			line(canvas[i],
				Point(j, rows),
				Point(j, rows - (histogram[i].at<int>(j) * rows / hmax[i])),
				((nChannels == 1) ? Scalar(255, 255, 255) : colors[i]),
				1, 8, 0
			);
		}
		// Biễu diễn đồ thị trên màn hình
		imshow("Histogram" + (char)(i), canvas[i]);
	}
}


/*
Hàm chuyển đổi ảnh hệ RGB (Red Green Blue) sang ảnh HSI (Hue Saturation Intensity)
Tham số đầu vào:
	Ảnh hệ RGB cần chuyển đổi
	Ảnh cần chuyển đổi sang hệ HSI

*/
void RGB2HSI(Mat inputImage, Mat inputImageHSI, float** H, float** S, float** I) {
	for (int row = 0; row < inputImage.rows; row++)
	{
		for (int col = 0; col < inputImage.cols; col++)
		{
			float blue = inputImage.at<Vec3b>(row, col)[0];
			float green = inputImage.at<Vec3b>(row, col)[1];
			float red = inputImage.at<Vec3b>(row, col)[2];
			float min_val = 0.0;
			min_val = min(red, min(blue, green));

			float s = 1 - 3 * (min_val / (blue + green + red));
			float in = (blue + green + red) / 3;
			s = (s < 0.00001) ? (0.0) : ((s > 0.99999) ? 1.0 : s);
			float h;

			if (s != 0.0)
			{
				h = 0.5 * ((red - green) + (red - blue)) / sqrt(((red - green) * (red - green)) + ((red - blue) * (green - blue)));
				h = acos(h);
				h = (blue <= green) ? h : ((360 * 3.14159265) / 180.0) - h;
			}
			else {
				h = 0.0;
			}

			inputImageHSI.at<Vec3b>(row, col)[0] = H[row][col] = (h * 180) / 3.14159265;
			inputImageHSI.at<Vec3b>(row, col)[1] = S[row][col] = s * 100;
			inputImageHSI.at<Vec3b>(row, col)[2] = I[row][col] = in;
		}

	}
}


/*
Hàm chuyển đổi ảnh hệ HSI (Hue Saturation Intensity) thành ảnh hệ RGB (Red Green Blue)
Tham số:
	Ảnh đầu vào hệ HSI cần chuyển đổi
	Ảnh đầu ra hệ RGB
	Mảng H
	Mảng S
	Mảng I
*/
void HSI2RGB(Mat outputImage, Mat inputImageHSI, float** H, float** S, float** I) {
	float r, g, b, h, s, in;

	for (int i = 0; i < inputImageHSI.rows; i++) {
		for (int j = 0; j < inputImageHSI.cols; j++) {
			h = H[i][j];
			s = S[i][j] / 100;
			in = I[i][j];

			if (h >= 0.0 && h < 120.0) {
				b = in * (1 - s);
				r = in * (1 + (s * cos(h * 3.14159265 / 180.0) / cos((60 - h) * 3.14159265 / 180.0)));
				g = 3 * in - (r + b);
			}
			else if (h >= 120.0 && h < 240.0) {
				h = h - 120;
				r = in * (1 - s);
				g = in * (1 + (s * cos(h * 3.14159265 / 180.0) / cos((60 - h) * 3.14159265 / 180.0)));
				b = 3 * in - (r + g);
			}
			else {
				h = h - 240;
				g = in * (1 - s);
				b = in * (1 + (s * cos(h * 3.14159265 / 180.0) / cos((60 - h) * 3.14159265 / 180.0)));
				r = 3 * in - (g + b);
			}

			if (b < 0) b = 0;
			if (b > 255) b = 255;

			if (g < 0) g = 0;
			if (g > 255) g = 255;

			if (r < 0) r = 0;
			if (r > 255) r = 255;

			outputImage.at<Vec3b>(i, j)[0] = round(b);
			outputImage.at<Vec3b>(i, j)[1] = round(g);
			outputImage.at<Vec3b>(i, j)[2] = round(r);
		}
	}
	return;
}


void ColorTransformer::CalcHistogram(const Mat& sourceImage, float histogram[]) {
	if (sourceImage.type() == CV_8UC1) {
		int size = sourceImage.rows * sourceImage.cols;
		calHistogram(sourceImage, histogram, size);
	}
	else {
		Mat inputImageHSI(sourceImage.rows, sourceImage.cols, sourceImage.type());

		// Khởi tạo 3 mảng lưu trữ giá trị Hue - Saturation - Initensy
		float** H = new float* [sourceImage.rows];
		float** S = new float* [sourceImage.rows];
		float** I = new float* [sourceImage.rows];

		for (int i = 0; i < sourceImage.rows; i++) {
			H[i] = new float[sourceImage.cols];
			S[i] = new float[sourceImage.cols];
			I[i] = new float[sourceImage.cols];
		}

		RGB2HSI(sourceImage, inputImageHSI, H, S, I);

		intensity2Histogram(I, histogram, sourceImage.rows, sourceImage.cols);
	}
}


/*
Hàm cân bằng lược đồ màu tổng quát cho ảnh bất kỳ
Tham so :
	sourceImage : ảnh ban đầu
	destinationImage : ảnh kết quả
Hàm trả về:
	1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
	0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
*/
int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage) {
	// Nếu ảnh có 1 kênh màu = ảnh xám
	if (sourceImage.type() == CV_8UC1) {
		float histogram[256];
		CalcHistogram(sourceImage, histogram);

		float tranFunc[256];
		calTransferFunction(histogram, tranFunc);

		float outHistogram[256];
		intensityMapping(tranFunc, outHistogram);

		//	destinationImage = sourceImage.clone();

		for (int y = 0; y < sourceImage.rows; y++) {
			for (int x = 0; x < sourceImage.cols; x++) {
				destinationImage.at<uchar>(y, x) =
					saturate_cast<uchar>(saturate_cast<int>(outHistogram[sourceImage.at<uchar>(y, x)]));
			}
		}
	}
	else // Nếu ảnh có 3 kênh màu = ảnh RGB 
	{
		// Khởi tạo ảnh để chuyển đổi sang không gian HSI
		Mat inputImageHSI(sourceImage.rows, sourceImage.cols, sourceImage.type());

		// Khởi tạo 3 mảng lưu trữ giá trị Hue - Saturation - Initensy
		float** H = new float* [sourceImage.rows];
		float** S = new float* [sourceImage.rows];
		float** I = new float* [sourceImage.rows];

		for (int i = 0; i < sourceImage.rows; i++) {
			H[i] = new float[sourceImage.cols];
			S[i] = new float[sourceImage.cols];
			I[i] = new float[sourceImage.cols];
		}

		RGB2HSI(sourceImage, inputImageHSI, H, S, I);
		float histogram[256];
		intensity2Histogram(I, histogram, sourceImage.rows, sourceImage.cols);

		float tranFunc[256];
		calTransferFunction(histogram, tranFunc);

		float outHistogram[256];
		intensityMapping(tranFunc, outHistogram);
		float** outI = new float* [sourceImage.rows];

		for (int i = 0; i < sourceImage.rows; i++) {
			outI[i] = new float[sourceImage.cols];
		}

		for (int i = 0; i < sourceImage.rows; i++) {
			for (int j = 0; j < sourceImage.cols; j++) {
				outI[i][j] = (int)outHistogram[(int)I[i][j]];
			}
		}

		// Khởi tạo ảnh đích với cùng kích thước của ảnh input - không gian RGB
		destinationImage = Mat(sourceImage.rows, sourceImage.cols, sourceImage.type());
		// Chuyển đổi ảnh HSI sang ảnh đích RGB sau khi đã cân bằng histogram
		HSI2RGB(destinationImage, inputImageHSI, H, S, outI);
	}

	// Chuyển đổi ảnh thành công
	return 1;
};

/*
Hàm so sánh hai ảnh
Tham so :
	image1 : ảnh thứ nhất
	image2 : ảnh thứ hai
Hàm trả về:
	độ đo sự tương đồng giữa hai ảnh
*/
float ColorTransformer::CompareImage(const Mat& image1, Mat& image2) {
	// Khởi tạo mảng tính giá trị histogram của ảnh 1
	float histogramImage1[256];
	// Gọi hàm tính giá trị histogram của ảnh 1
	CalcHistogram(image1, histogramImage1);

	// Khởi tạo mảng tính giá trị histogram của ảnh 2
	float histogramImage2[256];
	// Gọi hành tính giá trị histogram của ảnh 2
	CalcHistogram(image2, histogramImage2);

	// Giá trị delta tính lượng chênh lệnh giữa 2 ảnh
	float delta = 0;

	// Áp dụng công thức chi - bình phương để tính tổng độ chênh lệnh giữa 2 ảnh dựa và histogram của mỗi ảnh
	for (int i = 0; i < 256; i++) {
		if (histogramImage1[i] != 0)
			delta += (histogramImage1[i] - histogramImage2[i]) * (histogramImage1[i] - histogramImage2[i]) / (histogramImage1[i]);
	}

	// Trả về giá trị chênh lệnh của 2 ảnh
	return delta;
};


