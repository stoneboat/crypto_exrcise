#include "stdafx.h"
void hybridImage_demo() {
	Mat imgL = imread("marilyn.png", CV_8UC1);
	Mat imgH = imread("einstein.png", CV_8UC1);
	Mat imgL2(imgL.rows, imgL.cols, CV_16S);
	Mat imgH2(imgH.rows, imgH.cols, CV_16S);
	GaussianBlur(imgL, imgL2, Size(25, 25), 0);
	imgL2 = imgL2 - 127;
	imgL2.convertTo(imgL2, CV_16S, 255);
	Laplacian(imgH, imgH2, CV_16S, 3, 31);
	Mat dst;
	addWeighted(imgL2, 0.3, imgH2, 2, 0.0, dst);
	imshow("ss", dst);
}