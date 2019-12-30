#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include "Blur.h"
#include "Convolution.h"
#include "EdgeDetector.h"

using namespace cv;
using namespace std;

bool cmdCompare(char* cmd, string type) {
	for (int len = 0; len < type.size(); len++) {
		if (cmd[len] != type[len])
			return false;
	}
	return true;
}

int main(int argc, char* argv[])
{
	Mat inputImage = imread(argv[2], IMREAD_ANYCOLOR);
	cvtColor(inputImage, inputImage, COLOR_RGBA2GRAY);
	Mat outputImage = inputImage.clone();

	if (cmdCompare(argv[1], "--sobel"))
	{
		EdgeDetector func;
		func.DetectEdge(inputImage, outputImage, 1);
	}
	else if (cmdCompare(argv[1], "--prewitt"))
	{
		EdgeDetector func;
		func.DetectEdge(inputImage, outputImage, 2);
	} 
	else if (cmdCompare(argv[1], "--laplace"))
	{
		EdgeDetector func;
		func.DetectEdge(inputImage, outputImage,3);
	}

	namedWindow("Img");
	imshow("Img", outputImage);

	waitKey(0);
	return 0;
}