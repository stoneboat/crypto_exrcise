// CvWork.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <time.h>
#include <cmath>
# define M_PI           3.14159265358979323846  /* pi */
# define Squre(x,y) x*x+y*y
# define SqrRoot(x,y) sqrt(Squre(x,y))
const int base_ratio = 2;
const int ratio_slider_max = 10;
const int highThreshold_slider_max = 10;
double nms_lowvalue = 0;
double nms_highvalue = 1;
double lowThreshold;
double highThreshold;
int countHy = 0;
Mat link_dst;
Mat orientation;
int ratio;
int high_slide;
Mat getGaussian2DerivativeKernel(int size, double sigma);
Mat grayImageFilteringID(Mat filter, Mat src,double MAX_BRIGHTNESS);
enum towards{foward,backward};
enum orienWards{vertical,horizontal};
void masking(Mat& src, Mat&  des, double threshold) {
	Mat des_t;
	des_t = Mat_<double>(src.size());
	
	for (int i = 0; i < src.rows; i++) {
		double* f = src.ptr<double>(i);
		double* d = des_t.ptr<double>(i);
		for (int j = 0; j < src.cols; j++)
			if (f[j] > threshold)
				d[j] = f[j];
			else
				d[j] = 0;
	}
	des = des_t;
}
void MagnitudeD_x_y(Mat src, Mat X, Mat& Y);
void OrientationD_x_y(Mat des, Mat X, Mat Y);
void non_max_supp(const Mat& src, const Mat& ori,const int sz, Mat& dst, const double threshold);
void Hysteresis_analysis(Mat& src, const Mat& ori, Mat&dst, const double lowThreshold, const double hightThreshold);
Mat getGaussian2DerivativeKernel(int size , double sigma = 0) {
	assert(size % 2 == 1);
	if (sigma == 0)
		sigma = 0.3*((size - 1)*0.5 - 1) + 0.8;
	const double sigmaSqur = sigma*sigma;
	Mat gauDeravi(size, size, CV_64F);
	double coe = -double(1 / (2 * M_PI*sigmaSqur*sigmaSqur));
	for (int kx = -(size / 2); kx < size / 2 + 1; kx++) {
		double* f = gauDeravi.ptr<double>(uint(kx + size / 2));
		for (int ky = -(size / 2); ky < size / 2 + 1; ky++) {
			//gauDeravi.at<double>(uint(kx + size / 2), uint(ky + size / 2)) = coe*kx*exp(-(kx*kx + ky*ky) / (2 * sigma*sigma));
			f[uint(ky + size / 2)] = coe*kx*exp(-(kx*kx + ky*ky) / (2 * sigmaSqur));
		}
	}
		
	return gauDeravi;
}
Mat grayImageFilteringID(Mat src,Mat filter, double MAX_BRIGHTNESS = 255) {
	// INPUT: int src_img && double filter  OUTPUT : store in double MAT but decided by MAX_BRIGHTNESS
	assert(filter.type() == CV_32F || filter.type() == CV_64F);
	assert(filter.type() != CV_32F && filter.type() != CV_64F);
	double ** f = new double*[filter.rows];
	for (int i = 0; i < filter.rows; i++)
		f[i] = filter.ptr<double>(i);
	double min = DBL_MIN;
	double max = DBL_MAX;
	double pixel_val = 0.0;
	/*for (int x = 0; x < src.rows; x++) {
		int* I = src.ptr<int>(x);
		for (int y = 0; y < src.cols; y++) {
			pixel_val = 0.0;
			for()
		}
	}*/
	return Mat();
}
void MagnitudeD_x_y(Mat src,Mat X, Mat& Y) {
	assert(src.type() == CV_32F || src.type() == CV_64F);
	assert(X.type() == CV_32F || X.type() == CV_64F);
	assert(Y.type() == CV_32F || Y.type() == CV_64F);
	assert(src.rows == X.rows&& src.cols == X.cols);
	assert(X.rows == Y.rows && X.cols == Y.cols);
	for (int x = 0; x < src.rows; x++) {
		double* SF = src.ptr<double>(x);
		double* XF = X.ptr<double>(x);
		double* YF = Y.ptr<double>(x);
		for (int y = 0; y < src.cols; y++)
			SF[y] = sqrt(Squre(XF[y], YF[y]));
	}
		
}
void OrientationD_x_y(Mat des, Mat X, Mat Y) {
	assert(des.type() == CV_32F || des.type() == CV_64F);
	assert(X.type() == CV_32F || X.type() == CV_64F);
	assert(Y.type() == CV_32F || Y.type() == CV_64F);
	assert(des.rows == X.rows&& des.cols == X.cols);
	assert(X.rows == Y.rows && X.cols == Y.cols);
	for (int x = 0; x < des.rows; x++) {
		double* SF = des.ptr<double>(x);
		double* XF = X.ptr<double>(x);
		double* YF = Y.ptr<double>(x);
		for (int y = 0; y < des.cols; y++) {
			//SF[y] = atan2(YF[y], XF[y]);
			SF[y] = atan2( XF[y], YF[y]);
		}
	}
}
/* @brief supress non-maximal 
	
	@param src the input image/matrix, of CV_64F,CV_32F type
	@param sz the size of the window
	@param dst the mask of type of CV_64F, where non-zero elements corrspond to local maxima value of the src, the value of the given local maxima point has the same value as the coresponding point of the src
	@param mask an input matrix to skip particular elements
*/
struct double_Point {
	double x;
	double y;
	double_Point(double _x, double _y) {
		x = _x;
		y = _y;
	}
};
bool max_remain(Mat& src, double ori, int r, int c, double value) {
	// find the compared number's four neighbour using bilinear to estimate it's value
	double N = src.cols-1;
	double M = src.rows-1;
	double compared_value = 0.0;
	double radian = ori;// +(M_PI / 2);
	int orentation[2] = { 1,-1};
	Mat A = Mat_<double>(Size(2, 1));
	Mat B = Mat_<double>(Size(2, 2));
	Mat C = Mat_<double>(Size(1, 2));
	for (int k = 0; k < 2; k++) {
		/**
		binear interpolation
		**/
		/******************c*******/
		/*r  p0           p1*/
		/*                 */
		/*        p_       */
		/*                 */
		/*  p2           p3*/
		/*************************/
		//double_Point p_ = double_Point(orentation[k] * cos(radian) + c,r + orentation[k] * sin(radian));
		double_Point p_ = double_Point(orentation[k] * sin(radian) + c, r + orentation[k] * cos(radian));
		Point p0 = Point(max(0.0,floor(p_.x)), max(0.0,floor(p_.y)));
		Point p2 = Point(max(0.0, floor(p_.x)), min(N, ceil(p_.y)));
		Point p1 = Point(min(M, ceil(p_.x)), max(0.0, floor(p_.y)));
		Point p3 = Point(min(M, ceil(p_.x)), min(N, ceil(p_.y)));
		A.at<double>(0, 0) = 1 - (p_.x - p0.x) / (p1.x - p0.x);
		A.at<double>(0, 1) = (p_.x-p0.x)/(p1.x - p0.x);
		B.at<double>(0, 0) = src.at<double>(p2);
		B.at<double>(0, 1) = src.at<double>(p0);
		B.at<double>(1, 0) = src.at<double>(p3);
		B.at<double>(1, 1) = src.at<double>(p1);
		C.at<double>(0, 0) = 1 - (p_.y - p1.y) / (p3.y - p1.y);
		C.at<double>(1, 0) = (p_.y - p1.y) / (p3.y - p1.y);//p_.y - p3.y;
		Mat result = (A*B*C);
		compared_value = result.at<double>(0.0);
		//compared_value = (1.0 / ((p1.x - p2.x)*(p1.y - p3.y))) *(result.at<double>(0.0));
		if (compared_value > value)
			return false;
	}
	return true;
}
void non_max_supp(Mat& src, Mat& ori, const int sz, Mat& dst, const double threshold=0) {
	assert(src.type() == CV_32F || src.type() == CV_64F);
	int N = src.cols;
	int M = src.rows;
	dst = Mat_ < double>::zeros(src.size());
	for (int m = 2; m < M-2; m++) {
		double* f = src.ptr<double>(m);
		double* o = ori.ptr<double>(m);
		double* d = dst.ptr<double>(m);
		for (int n = 2; n < N-2; n++) {
			if (f[n] > threshold && max_remain(src, o[n], m, n, f[n]))
				d[n] = f[n];
		}
	}
}
void getHysteresisThreshold(Mat& src, const int highRatio, const int lowRatio, double* highThreshold, double* lowThreshold, const int totalRatio) {
	// inital the elements array
	//assert(highRatio > lowRatio);
	double* eleArray = new double[src.cols*src.rows];
	int count = 0;
	// get all of values larger then zeros elements
	for (int i = 0; i < src.rows; i++) {
		double * f = src.ptr<double>(i);
		for (int j = 0; j < src.cols; j++) {
			if (abs(f[j]) > 0.00001)
				eleArray[count++] = f[j];
		}
	}
	// sort elements array
	std::vector<double> elevector(eleArray, eleArray + count);
	std::sort(elevector.begin(),elevector.end());
	// set value of low threshold and high threshold
	*lowThreshold = *(elevector.begin() + count / totalRatio * lowRatio);
	*highThreshold = *(elevector.begin() + count / totalRatio * (totalRatio-highRatio));

	//destruct the elements array
	delete[] eleArray;

}
/*@brief given the specific Point (raxis,caxis) in src, find the next point connected by the Point corrsponding to orentation, than use the raxis and caxis store the return value
*/
void Hysteresis_getNextPointVer1(double oren, int* raxis, int* caxis,towards wards) {
	double radian = 0.0;
	if (wards == foward) {
		radian = oren+ M_PI/2;
	}
	else {
		radian = oren - M_PI/2;
	}
	double max = 0.f;
	radian = radian - M_PI / 2;
	Point p_ = Point(0,0);
	for (int i = 0; i < 5; i++) {
		Point tmp;
		radian += M_PI / 4;
		tmp.x = round(sin(radian)) + *caxis;
		tmp.y = *raxis - round(cos(radian)); //change
		if (link_dst.at<double>(tmp) > max)
			p_ = tmp;
	}
	*caxis = p_.x;
	*raxis = p_.y;
}
void Hysteresis_getNextPointHor1(double oren, int* raxis, int* caxis,towards wards) {
	double radian = oren;
	if (wards == foward) {
		radian = oren;
	}
	else {
		radian = oren - M_PI;
	}
	double max = 0.f;
	radian = radian - M_PI / 2;
	Point p_ = Point(0, 0);
	for (int i = 0; i < 5; i++) {
		Point tmp;
		radian += M_PI / 4;
		tmp.x = round(sin(radian)) + *caxis;
		tmp.y = *raxis - round(cos(radian)); //change
		if (link_dst.at<double>(tmp) > max)
			p_ = tmp;
	}
	//double_Point p_ = double_Point(round(cos(radian)) + *caxis, *raxis - round(sin(radian))); //change
	*caxis = p_.x;
	*raxis = p_.y;
}
void Hysteresis_getNextPointVer2(double oren, int* raxis, int* caxis, towards wards) {
	double radian = 0.0;
	if (wards == foward) {
		radian = oren + M_PI / 2;
	}
	else {
		radian = oren - M_PI / 2;
	}
	double max = 0.f;
	radian = radian - M_PI / 2;
	Point p_ = Point(0, 0);
	for (int i = 0; i < 5; i++) {
		Point tmp;
		radian += M_PI / 4;
		tmp.x = round(sin(radian)) + *caxis;
		tmp.y = *raxis - round(cos(radian)); //change
		if (link_dst.at<double>(tmp) > max)
			p_ = tmp;
	}
	//double_Point p_ = double_Point(round(cos(radian)) + *caxis, *raxis - round(sin(radian))); //change
	*caxis = p_.x;
	*raxis = p_.y;
}
void Hysteresis_getNextPointHor2(double oren, int* raxis, int* caxis, towards wards) {
	double radian = oren;
	if (wards == foward) {
		radian = oren;
	}
	else {
		radian = oren - M_PI;
	}
	double max = 0.f;
	radian = radian - M_PI / 2;
	Point p_ = Point(0, 0);
	for (int i = 0; i < 5; i++) {
		Point tmp;
		radian += M_PI / 4;
		tmp.x = round(sin(radian)) + *caxis;
		tmp.y = *raxis - round(cos(radian)); //change
		if (link_dst.at<double>(tmp) > max)
			p_ = tmp;
	}
	//double_Point p_ = double_Point(round(sin(radian)) + *caxis, *raxis - round(cos(radian))); //change																						  //double_Point p_ = double_Point(round(sin(radian)) + *caxis, *raxis + round(cos(radian))); //change
	*caxis = p_.x;
	*raxis = p_.y;
}
bool strongLinking(Mat& src, int raxis, int caxis, int r2axis, int c2axis) {// true for 1, false for 2
	if (caxis < 0 || raxis < 0 || raxis >= src.rows || caxis >= src.cols)
		return false;
	if (c2axis < 0 || r2axis < 0 || r2axis >= src.rows || c2axis >= src.cols)
		return true;
	if (src.at<double>(raxis, caxis) > src.at<double>(r2axis, c2axis))
		return true;
	else
		return false;
}
void Hysteresis_weaklink1(Mat& dst,Mat& src, const Mat& ori, int raxis, int caxis, const double& lowThreshold, const double& hightThreshold, towards wards, orienWards oriwards) {
	if (caxis < 0 || raxis < 0 || raxis >= src.rows || caxis >= src.cols)
		return;
	double tmp = src.at<double>(raxis, caxis);
	if (tmp<=lowThreshold)
		return;
	dst.at<double>(raxis, caxis) = 255;
	src.at<double>(raxis, caxis) = 0;
	int rnext = raxis;
	int cnext = caxis;
	if (wards == vertical) {
		if (oriwards == foward) {
			Hysteresis_getNextPointVer1(ori.at<double>(raxis, caxis), &rnext, &cnext, foward);
		}
		else {
			Hysteresis_getNextPointVer1(ori.at<double>(raxis, caxis), &rnext, &cnext, backward);
		}
	}
	else {
		if (oriwards == foward) {
			Hysteresis_getNextPointHor1(ori.at<double>(raxis, caxis), &rnext, &cnext, foward);
		}
		else {
			Hysteresis_getNextPointHor1(ori.at<double>(raxis, caxis), &rnext, &cnext, backward);
		}
	}
	Hysteresis_weaklink1(dst, src, ori, rnext, cnext, lowThreshold, hightThreshold,wards,oriwards);
	src.at<double>(raxis, caxis) = tmp;
}
void Hysteresis_weaklinkCheck1(Mat& dst, Mat& src, const Mat& ori, int raxis, int caxis, const double& lowThreshold, const double& hightThreshold, towards wards, orienWards oriwards) {
	if (caxis < 0 || raxis < 0 || raxis >= src.rows || caxis >= src.cols)
		return;
	double tmp = src.at<double>(raxis, caxis);
	if (tmp <= lowThreshold)
		return;
	countHy++;
	src.at<double>(raxis, caxis) = 0;
	int rnext = raxis;
	int cnext = caxis;
	if (wards == vertical) {
		if (oriwards == foward) {
			Hysteresis_getNextPointVer1(ori.at<double>(raxis, caxis), &rnext, &cnext,foward);
		}
		else {
			Hysteresis_getNextPointVer1(ori.at<double>(raxis, caxis), &rnext, &cnext, backward);
		}
	}
	else {
		if (oriwards == foward) {
			Hysteresis_getNextPointHor1(ori.at<double>(raxis, caxis), &rnext, &cnext,foward);
		}
		else {
			Hysteresis_getNextPointHor1(ori.at<double>(raxis, caxis), &rnext, &cnext, backward);
		}
	}
	Hysteresis_weaklinkCheck1(dst, src, ori, rnext, cnext, lowThreshold, hightThreshold,wards,oriwards);
	src.at<double>(raxis, caxis) = tmp;
}
void Hysteresis_weaklink2(Mat& dst, Mat& src, const Mat& ori, int raxis, int caxis, const double& lowThreshold, const double& hightThreshold, towards wards, orienWards oriwards) {
	if (caxis < 0 || raxis < 0 || raxis >= src.rows || caxis >= src.cols)
		return;
	double tmp = src.at<double>(raxis, caxis);
	if (tmp <= lowThreshold)
		return;
	dst.at<double>(raxis, caxis) = 255;
	src.at<double>(raxis, caxis) = 0;
	int rnext = raxis;
	int cnext = caxis;
	if (wards == vertical) {
		if (oriwards == foward) {
			Hysteresis_getNextPointVer2(ori.at<double>(raxis, caxis), &rnext, &cnext, foward);
		}
		else {
			Hysteresis_getNextPointVer2(ori.at<double>(raxis, caxis), &rnext, &cnext, backward);
		}
	}
	else {
		if (oriwards == foward) {
			Hysteresis_getNextPointHor2(ori.at<double>(raxis, caxis), &rnext, &cnext, foward);
		}
		else {
			Hysteresis_getNextPointHor2(ori.at<double>(raxis, caxis), &rnext, &cnext, backward);
		}
	}
	Hysteresis_weaklink2(dst, src, ori, rnext, cnext, lowThreshold, hightThreshold, wards, oriwards);
	src.at<double>(raxis, caxis) = tmp;
}
void Hysteresis_weaklinkCheck2(Mat& dst, Mat& src, const Mat& ori, int raxis, int caxis, const double& lowThreshold, const double& hightThreshold, towards wards, orienWards oriwards) {
	if (caxis < 0 || raxis < 0 || raxis >= src.rows || caxis >= src.cols)
		return;
	double tmp = src.at<double>(raxis, caxis);
	if (tmp <= lowThreshold)
		return;
	countHy++;
	src.at<double>(raxis, caxis) = 0;
	int rnext = raxis;
	int cnext = caxis;
	if (wards == vertical) {
		if (oriwards == foward) {
			Hysteresis_getNextPointVer2(ori.at<double>(raxis, caxis), &rnext, &cnext, foward);
		}
		else {
			Hysteresis_getNextPointVer2(ori.at<double>(raxis, caxis), &rnext, &cnext, backward);
		}
	}
	else {
		if (oriwards == foward) {
			Hysteresis_getNextPointHor2(ori.at<double>(raxis, caxis), &rnext, &cnext, foward);
		}
		else {
			Hysteresis_getNextPointHor2(ori.at<double>(raxis, caxis), &rnext, &cnext, backward);
		}
	}
	Hysteresis_weaklinkCheck2(dst, src, ori, rnext, cnext, lowThreshold, hightThreshold, wards, oriwards);
	src.at<double>(raxis, caxis) = tmp;
}
void Hysteresis_analysis(Mat& src,const Mat& ori, Mat&dst, const double lowThreshold, const double hightThreshold) {
	assert(src.size() == ori.size());
	int N = src.cols;
	int M = src.rows;
	for (int i = 0; i < M; i++) {
		double* f = src.ptr<double>(i);
		double* d = dst.ptr<double>(i);
		for (int j = 0; j < N; j++) {
			if (f[j] < hightThreshold)
				continue;
			else
				countHy = 0;
			// filling the threshold gap towarding the DOWN direction
			int currowAxis = i;
			int curcolAxis = j;
			Hysteresis_weaklinkCheck1(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold,foward,vertical);
			Hysteresis_weaklinkCheck1(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, backward, vertical);
			if (countHy > 3) {
				d[j] = 255;
				Hysteresis_weaklink1(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, foward, vertical);
				Hysteresis_weaklink1(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, backward, vertical);
			}
			/*countHy = 0;
			Hysteresis_weaklinkCheck1(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, foward, horizontal);
			Hysteresis_weaklinkCheck1(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, backward, horizontal);
			if (countHy > 3) {
				d[j] = 255;
				Hysteresis_weaklink1(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, foward, horizontal);
				Hysteresis_weaklink1(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, backward, horizontal);
			}*/
			/*countHy = 0;
			Hysteresis_weaklinkCheck2(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, foward, vertical);
			Hysteresis_weaklinkCheck2(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, backward, vertical);
			if (countHy > 3) {
				d[j] = 255;
				Hysteresis_weaklink2(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, foward, vertical);
				Hysteresis_weaklink2(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, backward, vertical);
			}*/
			/*countHy = 0;
			Hysteresis_weaklinkCheck2(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, foward, horizontal);
			Hysteresis_weaklinkCheck2(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, backward, horizontal);
			if (countHy > 3) {
				d[j] = 255;
				Hysteresis_weaklink2(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, foward, horizontal);
				Hysteresis_weaklink2(dst, src, ori, currowAxis, curcolAxis, lowThreshold, hightThreshold, backward, horizontal);
			}*/
		}
	}
}
/**
* @function on_trackbar
* @brief Callback for trackbar
*/
void on_trackbar(int,void*)
{
	double hystersisRatio = base_ratio + double(ratio) / ratio_slider_max;
	//assert((nms_highvalue / hystersisRatio) >= nms_lowvalue);
	getHysteresisThreshold(link_dst, high_slide, high_slide /hystersisRatio,&highThreshold,&lowThreshold, ratio_slider_max);
	//highThreshold = ((nms_highvalue / hystersisRatio - nms_lowvalue) / highThreshold_slider_max*high_slide + nms_lowvalue)*hystersisRatio;
	//lowThreshold = highThreshold/ hystersisRatio;
	//assert(lowThreshold >= nms_lowvalue);
	Mat hysterMat = Mat_<double>::zeros(link_dst.size());
	Hysteresis_analysis(link_dst, orientation, hysterMat, lowThreshold, highThreshold);
	imshow("hystersis", hysterMat);
	imwrite("hystersis.jpg", hysterMat);
	int i = 0;
}
//int main() {
//	// Filter with x, y derivatives of Gaussian
//	Mat compare = imread("compare.png", 0);
//	Mat gau_dx2 = getGaussian2DerivativeKernel(3);
//	Mat gau_dy2 = gau_dx2.t();
//	// calculate magnitude of gradient
//	Mat lena = imread("lena.png", 0);
//	GaussianBlur(lena, lena, Size(3, 3), 0, 0);
//	Mat lena_dx2(lena.rows, lena.cols, CV_64F);
//	Mat lena_dy2(lena.rows, lena.cols, CV_64F);
//	filter2D(lena, lena_dx2, CV_64F, gau_dx2);
//	//normalize(lena_dx2, lena_dx2, 0, 1, NORM_MINMAX);
//	filter2D(lena, lena_dy2, CV_64F, gau_dy2);
//	Mat gradientMagnitude(lena.rows, lena.cols, CV_64F);
//	MagnitudeD_x_y(gradientMagnitude, lena_dx2, lena_dy2);
//	GaussianBlur(gradientMagnitude, gradientMagnitude, Size(3, 3), 0, 0);
//	normalize(gradientMagnitude, gradientMagnitude, 0, 1, NORM_MINMAX);
//	orientation = Mat_<double>(lena.size());
//	OrientationD_x_y(orientation, lena_dx2, lena_dy2);
//	Mat nms_dst;
//	Mat mask = (gradientMagnitude > 0.045);
//	non_max_supp(gradientMagnitude, orientation, 1, nms_dst);// , 0.015);
//	normalize(nms_dst, nms_dst, 0, 1, NORM_MINMAX);
//	imshow("after nmx ", nms_dst);
//	imwrite("after_nmx.jpg", nms_dst*255);
//	link_dst = nms_dst;
//	//mask = link_dst > 0.0001;
//	minMaxLoc(link_dst, &nms_lowvalue, &nms_highvalue, NULL, NULL);
//	//hysterisi linking
//	ratio = 0;
//	high_slide = 3;
//	namedWindow("hystersis", 1);
//	createTrackbar("ratio 2-3", "hystersis", &ratio, ratio_slider_max, on_trackbar);
//	createTrackbar("highthres", "hystersis", &high_slide, highThreshold_slider_max, on_trackbar);
//	on_trackbar(ratio, 0);
//	cv::waitKey();
//	return 0;
//}

