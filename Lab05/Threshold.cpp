#include "Threshold.h"


//phân ngưỡng tĩnh
//class StaticThreshold

StaticThreshold::StaticThreshold(int low, int high) {
	_lowThreshold = low;
	_highThreshold = high;
};
StaticThreshold::~StaticThreshold() {
};
	
int StaticThreshold::Apply(const Mat& srcImage, Mat& dstImage) {
	// Nếu ảnh input đầu vào không phải là ảnh RGB thì chuyển thất bại
	// Kiểm tra điều kiện ngưỡng ảnh
	if (srcImage.type() != CV_8UC1 || _lowThreshold < 0 ||  _highThreshold > 255 ||  _lowThreshold > _highThreshold) {
		return 0;
	}


	// Khởi tạo ảnh output với kích thước của ảnh input, số kênh màu là 1 
	dstImage = srcImage.clone();

	// Duyệt qua tất cả các pixel của ảnh đầu vào
	for (int row = 0; row < srcImage.rows; row++)
	{
		for (int col = 0; col < srcImage.cols; col++)
		{
			uchar pixelVal = srcImage.at<uchar>(row, col);
			// Nếu pixel nằm trong vùng ngưỡng thì đó là object
			if (pixelVal > _lowThreshold&& pixelVal < _highThreshold)
				dstImage.at<uchar>(row, col) = 255;
			else // Ngược lại là background
				dstImage.at<uchar>(row, col) = 0;

		}
	}

	// Chuyển đổi ảnh thành công
	return 1;
};


int AverageLocalThreshold::Apply(const Mat& srcImage, Mat& dstImage, int winSize) {
	// Nếu ảnh input đầu vào không phải là ảnh Grayscale thì chuyển thất bại
	// Kiểm tra điều kiện cửa sổ ảnh và hệ số
	if (srcImage.type() != CV_8UC1 || winSize < 0 || _C < 0 || _C > 255) {
		return 0;
	}

	// Khởi tạo ảnh output với kích thước của ảnh input, số kênh màu là 1 
	dstImage = srcImage.clone();

	// Duyệt qua tất cả các pixel của ảnh đầu vào
	for (int row = 0; row < srcImage.rows; row++)
	{
		for (int col = 0; col < srcImage.cols; col++)
		{
			int cells = 0;
			int mean = 0;
			// Duyệt qua các khung cửa sổ với kích thước winSize
			for (int i = -(winSize / 2); i <= (winSize / 2); i++) {
				for (int j = -(winSize) / 2; j <= (winSize / 2); j++) {
					int curRow = row + i;
					int curCol = col + j;
					// Nếu nằm ngoài kích thước của ảnh
					if (curRow < 0 || curRow >= srcImage.rows) continue;
					if (curCol < 0 || curCol >= srcImage.cols) continue;
					// Biến cells đếm số lượng kích thước cửa sổ duyệt
					cells++;
					// Tổng giá trị pixel ảnh trong khung cửa sổ
					mean += srcImage.at<uchar>(curRow, curCol);
				}
			}
			// Lấy giá trị trung bình
			mean /= cells;
			// Gán cho pixel của ảnh đầu ra
			uchar pixelVal = srcImage.at<uchar>(row, col);
			if (pixelVal > mean - _C)
				dstImage.at<uchar>(row, col) = 255;
			else
				dstImage.at<uchar>(row, col) = 0;
		}
	}

	// Chuyển đổi ảnh thành công
	return 1;
};


AverageLocalThreshold::AverageLocalThreshold(int c) {
	this->_C = c;
};

AverageLocalThreshold::~AverageLocalThreshold() {
};



int MedianLocalThreshold::Apply(const Mat& srcImage, Mat& dstImage, int winSize) {
	// Nếu ảnh input đầu vào không phải là ảnh Grayscale thì chuyển thất bại
	// Kiểm tra điều kiện kích thước cửa sổ winSize và giá trị hệ số _C
	if (srcImage.type() != CV_8UC1 ||  winSize < 0 || _C < 0 || _C > 255 ) {
		return 0;
	}

	// Khởi tạo ảnh output với kích thước của ảnh input, số kênh màu là 1 
	dstImage = srcImage.clone();

	// Duyệt qua tất cả các pixel của ảnh đầu vào
	for (int row = 0; row < srcImage.rows; row++)
	{
		for (int col = 0; col < srcImage.cols; col++)
		{
			vector <uchar> temp;
			for (int i = -(winSize / 2); i <= (winSize / 2); i++) {
				for (int j = -(winSize) / 2; j <= (winSize / 2); j++) {
					int curRow = row + i;
					int curCol = col + j;
					if (curRow < 0 || curRow >= srcImage.rows) continue;
					if (curCol < 0 || curCol >= srcImage.cols) continue;
					temp.push_back(srcImage.at<uchar>(curRow, curCol));
				}
			}
			// Hàm nth_element dùng để sắp xếp mảng lại theo thứ tự vị trí size/2 sẽ là trung vị
			// Phía trái trung vị sẽ là các phần tử nhỏ hơn nó
			// Phía phải trung vị là các phần tử lớn hơn nó
			nth_element(temp.begin(), temp.begin() + temp.size() / 2, temp.end());

			// Lấy giá trị trung vị
			uchar median = temp[temp.size() / 2];
			
			// So sánh để xác định object và background
			uchar pixelVal = srcImage.at<uchar>(row, col);
			if (pixelVal > median - _C)
				dstImage.at<uchar>(row, col) = 255;
			else
				dstImage.at<uchar>(row, col) = 0;
		}
	}

	// Chuyển đổi ảnh thành công
	return 1;
};


MedianLocalThreshold::MedianLocalThreshold(int c) {
	this->_C = c;
};
MedianLocalThreshold::~MedianLocalThreshold() {
};

int SauvolaLocalThreshold::Apply(const Mat& srcImage, Mat& dstImage, int winSize) {
	// Nếu ảnh input đầu vào không phải là ảnh Grayscale thì chuyển thất bại
	// Kiểm tra giá trị _k và _r
	if (srcImage.type() != CV_8UC1 || _k < 0 || _k > 1 || _r < 0 || _r > 255 ) {
		return 0;
	}

	// Khởi tạo ảnh output với kích thước của ảnh input, số kênh màu là 1 
	dstImage = srcImage.clone();

	// Duyệt qua tất cả các pixel của ảnh đầu vào
	for (int row = 0; row < srcImage.rows; row++)
	{
		for (int col = 0; col < srcImage.cols; col++)
		{
			vector <uchar> temp;
			temp.clear();
			int sum = 0;

			// Duyệt qua các khung cửa sổ
			for (int i = -(winSize / 2); i <= (winSize / 2); i++) {
				for (int j = -(winSize) / 2; j <= (winSize / 2); j++) {
					int curRow = row + i;
					int curCol = col + j;
					// Kiểm tra nếu nằm ngoài vùng kích thước của ảnh
					if (curRow < 0 || curRow >= srcImage.rows) continue;
					if (curCol < 0 || curCol >= srcImage.cols) continue;
					// Thêm giá trị vào vector cửa sổ ảnh hiện tại
					temp.push_back(srcImage.at<uchar>(curRow, curCol));
					// Tính tổng của cửa sổ
					sum += srcImage.at<uchar>(curRow, curCol);
				}
			}

			// Lấy giá trị trung bình 
			int mean = sum / temp.size();

			// Tính giá trị phương sai: Đặc trưng cho độ phân tán của dữ liệu so với giá trị trung bình
			// Phương sai càng nhỏ -> Dữ liệu càng gần với giá trị trung bình
			// Tính tổng bình phương các điểm dữ liệu trừ đi giá trị trung bình 
			float variance = 0;
			for (int i = 0; i < temp.size(); i++) {
				variance += (temp[i] - mean) * (temp[i] - mean);
			}
			// Chia cho n-1 số điểm dữ liệu
			variance /= (temp.size() - 1);
			// Tính giá trị độ lệch chuẩn: Giá trị chênh lệch trong tập dữ liệu so với giá trị trung bình đã tính ra
			// Nếu độ lệch chuẩn = 0 -> phương sai = 0 -> các giá trị = trung bình -> không có sự biến thiên
			// Nếu độ lệch chuẩn càng lớn -> sự biến thiên xung quanh giá trị trung bình càng lớn
			float standardDeviation = sqrt(variance);

			// So sánh và gán trị trị cho ảnh đầu ra
			uchar pixelVal = srcImage.at<uchar>(row, col);
			if (pixelVal > mean* (1 + _k * (standardDeviation / _r - 1)))
				dstImage.at<uchar>(row, col) = 255;
			else
				dstImage.at<uchar>(row, col) = 0;
		}
	}
	return 1;
};


SauvolaLocalThreshold::SauvolaLocalThreshold(int r, float k) {
	this->_r = r;
	this->_k = k;
};
SauvolaLocalThreshold::~SauvolaLocalThreshold() {
};