#include "stdafx.h"
#define maxoffset 21
const int kernel = 5;
#define max2D(a,b)  (a)>(b) ? a:b
#define min2D(a,b)  (a)>(b) ? b:a
#define squar(a) (a)*(a)
#define PEucluDis(p1,p2) sqrt(squar(p1[0]-p2[0])+squar(p1[1]-p2[1])+squar(p1[2]-p2[2]))
//int main() {
//	Mat imageL = imread("view0.png", 1);
//	Mat imageR = imread("view1.png", 1);
//	const int rows = imageL.rows;
//	const int cols = imageL.cols;
//	Mat Disparity = Mat(rows, cols, CV_8U);
//	for (int r = 0; r < imageL.rows; r++) {
//		for (int c = 0; c < imageL.cols; c++) {
//			int disparity = 0;
//			int max = INT_MAX;
//			for (int i = max2D(0, c - (maxoffset / 2)); i < (min2D(cols, c + (maxoffset / 2))); i++) {
//				int tmpV = 0;
//				//for (int k = max2D(0, i - kernel / 2); k<(min2D(cols, i + (kernel / 2))); k++) {
//				for (int k =- kernel / 2; k<(kernel / 2); k++){
//					if (c + k < 0 || c + k >= cols || i + k < 0 || i + k >= cols)
//						continue;
//					for (int m = -kernel / 2; m < kernel / 2; m++) {
//						if (r+m < 0 || r + m >= rows)
//							continue;
//						Vec3b point = imageL.at<Vec3b>(r+m, c + k);
//						Vec3b accordP = imageR.at<Vec3b>(r+m, i + k);
//						tmpV += PEucluDis(point, accordP);
//					}
//				}
//				if (max > tmpV) {
//					max = tmpV;
//					disparity = (c-i);
//				}
//			}
//			Disparity.at<uchar>(r, c) = disparity*(255/maxoffset);
//		}
//	}
//	imshow("Disparity Map", Disparity);
//	cv::waitKey(0);
//	return 0;
//}