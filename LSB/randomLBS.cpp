// CvWork.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <bitset>
#include <fstream>
# define M_PI           3.14159265358979323846 
# define Squre(x,y) x*x+y*y
# define SqrRoot(x,y) sqrt(Squre(x,y))

class LBSHiding {
public:
	LBSHiding(unsigned int _sessioinKey) {
		sessionKey = _sessioinKey;
		message = NULL;
		mLength = 0;
		samplingG = NULL;
	}
	void hiding(string _fileName, string _MatName) {
		fileName = _fileName;
		MatName = _MatName;
		/// get the secrete message and it's length in byte
		getMessage(fileName, message, mLength);
		/// get the hiding image and caculate it's magnitude
		Mat hideImage = imread(MatName, 0);
		unsigned long arrayLengh = mLength * sizeof(char) * 8 * repeatThreshold*interval;
		assert(arrayLengh < hideImage.rows*hideImage.cols);
		Point* samplingG = new Point[arrayLengh];
		getSamplingSet(hideImage, arrayLengh, samplingG);
		//using random LSB hide message
		//initial random key
		srand(sessionKey);
		hideMessage(hideImage, message, mLength, samplingG);
		imwrite("hideimage.png", hideImage);
		delete[] message;
		delete[] samplingG;
	}
	void extracting(string _MatName,string OriMatName) {
		MatName = _MatName;
		Mat hideImage = imread(MatName, 0);
		Point* samplingG = new Point[hideImage.rows*hideImage.cols];
		unsigned long setLength = 0;
		extractSamplingSet(OriMatName, setLength, samplingG);
		//using random LSB hide message
		//initial random key
		srand(sessionKey);
		mLength = setLength / (sizeof(char)*8);
		extractMessage(hideImage, message, samplingG,setLength);
		for (int i = 0; i < 100; i++)
			cout << message[i];
		int i = 4;
		delete[] message;
		delete[] samplingG;
	}
private:
	void getMessage(string fileName, char*& message, unsigned long& mLength);
	void getSamplingSet(Mat& src, unsigned long setLength, Point*&);
	Mat getGaussian2DerivativeKernel(int size, double  sigma = 0);
	void MagnitudeD_x_y(Mat src, Mat X, Mat& Y);
	void hideMessage(Mat src, char* message, unsigned long mLength, Point* samplingG);
	void extractSamplingSet(string srcMat_name,unsigned long& setLength,Point* samplingG);
	void extractMessage(Mat src,char*& message,Point* samplingG,const unsigned long setLength);
	const int repeatThreshold = 10;
	const int interval = 10;
	string MatName;
	Mat hideImage;
	string fileName;
	unsigned int sessionKey;
	unsigned long mLength;
	char* message;
	Point* samplingG;
};
struct magPoint {
	int x;
	int y;
	double value;
	magPoint(int _x = 0, int _y = 0, int _value = 0) {
		x = _x;
		y = _y;
		value = _value;
	}
};
bool compare_magPoint(const magPoint& a,const  magPoint& b)
{
	return a.value>b.value;   //descending order
}


int main()
{
	LBSHiding hiding = LBSHiding(783);
	//hiding.hiding("test.txt","lena.png" );
	hiding.extracting("hideimage.png", "lena.png");
	cv::waitKey(0);
    return 0;
}
void LBSHiding::getMessage(string fileName, char*& message, unsigned long& mLength) {
	FILE * f = fopen(fileName.c_str(), "rb");
	unsigned long ulSize = 0;
	assert(!fseek(f,0, SEEK_END)); /// move to end of file
	mLength = ftell(f);             /// calcute the size of file in byte
	assert(!fseek(f, 0, SEEK_SET));/// start to read message
	message = new char[mLength];
	fread(message, sizeof(char), mLength, f);
	fclose(f);                     /// close message file
}
void LBSHiding::getSamplingSet(Mat& src, unsigned long setLength, Point*& samplingG) {
	//get image magnitude
	Mat gau_dx2 = getGaussian2DerivativeKernel(3);
	Mat gau_dy2 = gau_dx2.t();
	Mat src_dx2 = Mat_<double>(src.size());
	Mat src_dy2 = Mat_<double>(src.size());
	filter2D(src, src_dx2, CV_64F, gau_dx2);
	filter2D(src, src_dy2, CV_64F, gau_dy2);
	Mat gradientMagnitude = Mat_<double>(src.size());
	MagnitudeD_x_y(gradientMagnitude, src_dx2, src_dy2);
	// sort magnitude point
	magPoint* eleArray = new magPoint[src.rows*src.cols];
	unsigned long count = 0;
	for (int i = 0; i < src.rows; i++) {
		double * f = gradientMagnitude.ptr<double>(i);
		for (int j = 0; j < src.cols; j++) {
			if (abs(f[j]) > 0.00001) {
				eleArray[count].x = j;
				eleArray[count].y = i;
				eleArray[count++].value = f[j];
			}
		}
	}
	std::vector<magPoint> elevector(eleArray, eleArray + count);
	std::sort(elevector.begin(), elevector.end(), compare_magPoint);
	// get subset
	for (int i = 0; i < setLength; i++) {
		samplingG[i].x = (elevector.begin() + i)->x;
		samplingG[i].y = (elevector.begin() + i)->y;
	}
	delete eleArray;
}
Mat LBSHiding::getGaussian2DerivativeKernel(int size, double sigma) {
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
void LBSHiding::MagnitudeD_x_y(Mat src, Mat X, Mat& Y) {
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
void LBSHiding::hideMessage(Mat src, char* message, unsigned long mLength, Point* samplingG) {
	unsigned long count = 0;
	bool cm;
	int hm;
	int rest = 0;
	for (int i = 0; i < repeatThreshold; i++) {
		for (int j = 0; j < mLength; j++) {
			for (int k = (sizeof(char) * 8)-1; k >=0; k--) {
				cm = (message[j] >> k) & 1;
				hm = src.at<uint8_t>(samplingG[count]);
				rest = (hm % 8);
				hm = (((hm >> 3) / 2 * 2) + int(cm)) << 3;
				hm = hm+rest;
				//hm = (hm / 2) * 2 + int(cm);
				src.at<uint8_t>(samplingG[count]) = hm;
				count = count + rand() % interval+1;
			}
		}
	}
}
void LBSHiding::extractSamplingSet(string srcMat_name, unsigned long& setLength, Point* samplingG) {
	//get image magnitude
	Mat src = imread(srcMat_name,0);
	Mat gau_dx2 = getGaussian2DerivativeKernel(3);
	Mat gau_dy2 = gau_dx2.t();
	Mat src_dx2 = Mat_<double>(src.size());
	Mat src_dy2 = Mat_<double>(src.size());
	filter2D(src, src_dx2, CV_64F, gau_dx2);
	filter2D(src, src_dy2, CV_64F, gau_dy2);
	Mat gradientMagnitude = Mat_<double>(src.size());
	MagnitudeD_x_y(gradientMagnitude, src_dx2, src_dy2);
	// sort magnitude point
	magPoint* eleArray = new magPoint[src.rows*src.cols];
	unsigned long count = 0;
	for (int i = 0; i < src.rows; i++) {
		double * f = gradientMagnitude.ptr<double>(i);
		for (int j = 0; j < src.cols; j++) {
			if (abs(f[j]) > 0.00001) {
				eleArray[count].x = j;
				eleArray[count].y = i;
				eleArray[count++].value = f[j];
			}
		}
	}
	std::vector<magPoint> elevector(eleArray, eleArray + count);
	std::sort(elevector.begin(), elevector.end(), compare_magPoint);
	// get set
	for (int i = 0; i < count; i++) {
		samplingG[i].x = (elevector.begin() + i)->x;
		samplingG[i].y = (elevector.begin() + i)->y;
	}
	setLength = count;
	delete eleArray;
}
void LBSHiding::extractMessage(Mat src, char*& message, Point* samplingG, const unsigned long setLength) {
	unsigned long count = 0;
	message = new char[setLength / (sizeof(char)*8)];
	int tmp = 0;
	bool cm;
	int hm;
	for (int i = 0; i < setLength/(sizeof(char)*8); i++) {
			for (int k = 0; k < (sizeof(char)*8); k++) {
				cm = ((src.at<uint8_t>(samplingG[count])>>3) & 1);
				tmp = (tmp << 1) + cm;
				count = count + rand() % interval+1;
				if (count > setLength)
					return;
			}
			message[i] = char(tmp);
			tmp = 0;
	}
}

