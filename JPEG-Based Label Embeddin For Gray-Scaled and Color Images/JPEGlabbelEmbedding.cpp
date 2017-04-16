// CvWork.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ImageCompare.h"
#include <ctime>
#include <cmath>
# define M_PI 3.14159265358979323846 /* pi */
typedef IMagePixelAnaysis::BasicIMagePixelAnaysis matAnayzier;
/// Luminance Quantization Table
enum quantizer{luminance, Chrominance};
enum copyType{int2double,double2int};
const int MD = 4;
const double D = 2;
const int lSQtable[8][8] = {
	16 , 11 , 10 , 16 , 24 , 40 , 51 , 61 ,
	12 , 12 , 14 , 19 , 26 , 58 , 60 , 55,
	14 , 13 , 16 , 24 , 40 , 57 , 69 , 56,
	14 , 17 , 22 , 29 , 51 , 87 , 80 , 62,
	18 , 22 , 37 , 56 , 68 ,109 ,103 , 77,
	24 , 35 , 55 , 64 , 81 ,104 ,113 , 92,
	49 , 64 , 78 , 87 ,103 ,121 ,120 ,101,
	72 , 92 , 95 , 98 ,112 ,100 ,103 , 99
};
const int CSQtable[8][8] = {
	17 , 18 ,24, 47, 99, 99 ,99, 99,
	18 ,21 ,26 ,66 ,99 ,99 ,99 ,99,
	24 ,26 ,56 ,99 ,99 ,99 ,99 ,99,
	47 ,66 ,99 ,99 ,99 ,99 ,99 ,99,
	99 ,99 ,99 ,99 ,99 ,99 ,99 ,99,
	99 ,99 ,99 ,99 ,99 ,99 ,99 ,99,
	99 ,99 ,99 ,99 ,99 ,99 ,99 ,99,
	99 ,99 ,99 ,99 ,99 ,99 ,99 ,99
};
const double YIQ2RGBTable[3][3] = {
	0.212, 0.523, 0.311,
	0.596, 0.275, 0.321,
	0.299, 0.587, 0.114
};
const int locationSet[18][3][2] = {
	0,2,1,1,1,2,
	1,1,0,2,1,2,
	0,3,1,2,1,3,
	1,2,0,3,1,3,
	1,1,0,2,1,2,
	0,2,1,1,1,2,
	1,1,2,0,0,2,
	2,0,1,1,0,2,
	0,2,1,1,2,0,
	1,1,0,2,2,0,
	1,2,2,1,0,3,
	2,1,1,2,0,3,
	1,2,0,3,2,1,
	0,3,1,2,2,1,
	1,1,2,0,2,1,
	2,0,1,1,2,1,
	1,2,2,1,2,2,
	2,1,1,2,2,2,
};
class blockMap {
public:
	blockMap& operator= (const blockMap& tmp) {
		row_length = tmp.row_length;
		col_length = tmp.col_length;
		blockmap = new bool*[row_length];
		for (int i = 0; i < col_length; i++)
			blockmap[i] = new bool[col_length];
		for (int i = 0; i < row_length; i++)
			for (int j = 0; j < col_length; j++)
				blockmap[i][j] = true;
		blockSet = Mat_<uint8_t>(tmp.blockSet.size());
		tmp.blockSet.copyTo(blockSet);
		return *this;
	}
	blockMap(const unsigned int _row_length, const unsigned int _col_length, Mat _blockSet) {
		row_length = _row_length;
		col_length = _col_length;
		blockmap = new bool*[row_length];
		for (int i = 0; i < col_length; i++)
			blockmap[i] = new bool[col_length];
		for (int i = 0; i < row_length; i++)
			for (int j = 0; j < col_length; j++)
				blockmap[i][j] = true;
		blockSet = Mat_<uint8_t>(_blockSet.size());
		_blockSet.copyTo(blockSet);
	}
	blockMap() {
		blockmap = NULL;
	}
	~blockMap() {
		if (blockmap != NULL) {
			for (int i = 0; i < row_length; i++)
				delete[] blockmap[i];
			delete[] blockmap;
		}
	}
	void getNextBlockNum(unsigned int& rowN, unsigned int& colN, int &randomNum);
	void diableBlock(unsigned int& rowN, unsigned int& colN);
private:
	Mat blockSet;
	unsigned int row_length;
	unsigned int col_length;
	bool** blockmap;
};
void blockMap::getNextBlockNum(unsigned int& row_num, unsigned int& col_num,int &randomNum) {
	while (true) {
		row_num = rand() % row_length;
		col_num = rand() % col_length;
		randomNum = (row_num*col_num) % 18;
		if (!blockmap[row_num][col_num])
			continue;
		else
			break;
	}
}
void blockMap::diableBlock(unsigned int& rowN, unsigned int& colN) {
	blockmap[rowN][colN] = false;
}
class GCEmbedding {
public:
	GCEmbedding(const string& _hiddenImageName,const string& _secreteFileName,const int _key) {
		secreteFileName = _secreteFileName;
		hiddenImageName = _hiddenImageName;
		key = _key;
	}
	GCEmbedding(const string& _hiddenImageName, const int _key) {
		hiddenImageName = _hiddenImageName;
		key = _key;
	}
	void hidding();
	void extracting();
private:
	void _extracting(const Mat& imat, Mat& des, char*& secereMess, const unsigned int smL );
	bool checkWrite(Mat& src,const unsigned int & rn, const unsigned int& cn,const int randomNum,const int ms);
	bool checkRead(Mat& src, const unsigned int & rn, const unsigned int& cn, const int randomNum);
	char* getMessageFlow(const string& fileName, unsigned int& mLength);
	void _hidding(const Mat& imat, Mat& des, char* secereMess, const unsigned int smL);
	void mergeThreeBasicImgToColor(Mat& OCImg, Mat& OBGImg, Mat& OGGImg, Mat& ORGImg);
	void converToYIQ(const Mat& imat, Mat& des);
	void PreNormalized(const Mat& imat, Mat& des);
	void PostNormalized(const Mat& imat, Mat& des);
	void FDTC(const Mat& imat, Mat &des);
	void IFDTC(const Mat& imat, Mat &des);
	void Quantization(const Mat& imat, Mat &des, quantizer quanti = luminance);
	void Quantization(Mat& src, Mat& des, const int* q[8]);
	void IQuantization(const Mat& imat, Mat &des, quantizer quanti = luminance);
	void IQuantization(Mat& src, Mat& des, const int* q[8]);
	void CopyTo(const Mat& src, Mat&des, copyType);//copy int type mat to double type mat
	void preProcess(const Mat& src, Mat& des, quantizer quanti = luminance);
	void postProcess(const Mat& src, Mat& des, quantizer quanti = luminance);
	void BDCT(Mat& src, Mat& des); //block decrete cosine transform
	void IBDCT(Mat& src, Mat& des); //block inverse decrete cosine transform
	double C(const unsigned int& i, const unsigned int&k) {
		double A = (k == 0) ? 1 / sqrt(2) : 1;
		return A*cos(M_PI*(2 * i + 1)*k / 16);
	}
	string secreteFileName;
	string hiddenImageName;
	char* secretMessege;
	unsigned int messegeLength;
	int key;
	Mat hiddenImage;
};
void GCEmbedding::_hidding(const Mat& imat, Mat& des, char* secereMess, const unsigned int smL) {
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	blockMap blockManager((src.rows / 8), (src.cols / 8), src);
	unsigned int RblockNum = 0;
	unsigned int CblockNum = 0;
	unsigned int secretByte = 0;
	int randomNum = 0;
	for (int i = 0; i < smL; i++) {
		unsigned int tmp = secereMess[i];
		for (int j = 7; j >= 0; j--) {
			secretByte = (tmp >> j);
			tmp = tmp - (secretByte << j);
			while (true) {
				blockManager.getNextBlockNum(RblockNum, CblockNum, randomNum);
				if (checkWrite(des,RblockNum, CblockNum, randomNum,secretByte)) {
					blockManager.diableBlock(RblockNum, CblockNum);
					break;
				}
				blockManager.diableBlock(RblockNum, CblockNum);
			}
			Mat local = des.rowRange(RblockNum * 8, RblockNum * 8 + 8).colRange(CblockNum * 8, CblockNum * 8 + 8);
			int line = randomNum;
			double a = local.at<double>(locationSet[line][0][0], locationSet[line][0][1]);
			double b = local.at<double>(locationSet[line][1][0], locationSet[line][1][1]);
			double c = local.at<double>(locationSet[line][2][0], locationSet[line][2][1]);
			if (secretByte == 1) {
				if (a < c + D)
					local.at<double>(locationSet[line][0][0], locationSet[line][0][1]) = c + D +1;
				if (b < c + D)
					local.at<double>(locationSet[line][1][0], locationSet[line][1][1]) = c + D +1 ;
			}
			if (secretByte == 0) {
				if (a > c - D)
					local.at<double>(locationSet[line][0][0], locationSet[line][0][1]) = c - D-1;
				if (b > c - D)
					local.at<double>(locationSet[line][1][0], locationSet[line][1][1]) = c - D-1;
			}
		}
	}
}
void GCEmbedding::_extracting(const Mat& imat, Mat& des, char*& secereMess, const unsigned int smL) { //要考虑获取信息长度的问题，这个是可以嵌入到信息中的，但是暂时不想考虑这个问题，这里直接赋值了
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	blockMap blockManager((src.rows / 8), (src.cols / 8), src);
	unsigned int RblockNum = 0;
	unsigned int CblockNum = 0;
	unsigned int secretByte = 0;
	int randomNum = 0;
	for (int i = 0; i < smL; i++) {
		int tmp = 0;
		for (int j = 7; j >= 0; j--) {
			while (true) {
				blockManager.getNextBlockNum(RblockNum, CblockNum, randomNum);
				if (checkRead(des, RblockNum, CblockNum, randomNum)) {
					blockManager.diableBlock(RblockNum, CblockNum);
					break;
				}
				blockManager.diableBlock(RblockNum, CblockNum);
			}
			Mat local = des.rowRange(RblockNum * 8, RblockNum * 8 + 8).colRange(CblockNum * 8, CblockNum * 8 + 8);
			int line = randomNum;
			double a = local.at<double>(locationSet[line][0][0], locationSet[line][0][1]);
			double b = local.at<double>(locationSet[line][1][0], locationSet[line][1][1]);
			double c = local.at<double>(locationSet[line][2][0], locationSet[line][2][1]);
			if (a < c  && b < c )
				secretByte = 0;
			if (a > c  && b > c)
				secretByte = 1;
			tmp = tmp + (secretByte << j);
		}
		secereMess[i] = tmp;
	}
	secereMess[smL] = '\0';
}
bool GCEmbedding::checkWrite(Mat& src,const unsigned int & rn, const unsigned int& cn,const int randomNum,const int ms) {
	Mat local = src.rowRange(rn * 8, rn * 8 + 8).colRange(cn * 8, cn * 8 + 8);
	int line = randomNum;
	double a = local.at<double>(locationSet[line][0][0], locationSet[line][0][1]);
	double b = local.at<double>(locationSet[line][1][0], locationSet[line][1][1]);
	double c = local.at<double>(locationSet[line][2][0], locationSet[line][2][1]);
	if (((a - c) > MD && (c-b) > MD)||((b-c) > MD && (c-a) > MD)) {
		return false;
	}
	if ((a - c) > MD&& ms==0) {
		local.at<double>(locationSet[line][1][0], locationSet[line][1][1]) = c - D;
		return false;
	}
	if ((b - c) > MD&& ms == 0) {
		local.at<double>(locationSet[line][0][0], locationSet[line][0][1]) = c - D;
		return false;
	}
	if ((c-a) > MD&& ms == 1) {
		local.at<double>(locationSet[line][1][0], locationSet[line][1][1]) = c + D;
		return false;
	}
	if ((c-b) > MD&& ms == 1) {
		local.at<double>(locationSet[line][0][0], locationSet[line][0][1]) = c + D;
		return false;
	}
	return true;
}
bool GCEmbedding::checkRead(Mat& src, const unsigned int & rn, const unsigned int& cn, const int randomNum) {
	Mat local = src.rowRange(rn * 8, rn * 8 + 8).colRange(cn * 8, cn * 8 + 8);
	int line = randomNum;
	double a = local.at<double>(locationSet[line][0][0], locationSet[line][0][1]);
	double b = local.at<double>(locationSet[line][1][0], locationSet[line][1][1]);
	double c = local.at<double>(locationSet[line][2][0], locationSet[line][2][1]);
	if ((a <= c&& c <= b) || (b <= c&&c <= a))
		return false;
	return true;
}
void GCEmbedding::hidding() {
	hiddenImage = imread(hiddenImageName);
	secretMessege = getMessageFlow(secreteFileName, messegeLength);
	srand(key);
	if (hiddenImage.channels() == 3) {
		converToYIQ(hiddenImage,hiddenImage);
		Mat des[3];
		Mat MChrom[3]; // 0 for luminance 1 for Yb 2 for Yr
		split(hiddenImage, MChrom);
		//preProcess(MChrom[0], des[0],luminance);
		//preProcess(MChrom[1], des[1], Chrominance);
		preProcess(MChrom[2], des[2], Chrominance);
		/*************cryptoblock  Start*************/
		_hidding(des[2], des[2], secretMessege, messegeLength);
		/*************cryptoblock  End*************/
		Mat recoverDes[3];
		recoverDes[0] = MChrom[0];
		recoverDes[1] = MChrom[1];
		//postProcess(des[0], recoverDes[0], luminance);
		//postProcess(des[1], recoverDes[1], Chrominance);
		postProcess(des[2], recoverDes[2], Chrominance);
		for (int i = 0; i < 3; i++)
			recoverDes[i] = Mat_<uchar>(recoverDes[i]);
		Mat result;
		mergeThreeBasicImgToColor(result, recoverDes[0], recoverDes[1], recoverDes[2]);
		cvtColor(result, result, CV_YCrCb2BGR);
		imwrite("test.jpg", result);
	}
	else {
		Mat des;
		preProcess(hiddenImage, des);
		/*************cryptoblock  Start*************/
		_hidding(des, des, secretMessege, messegeLength);
		/*************cryptoblock  End*************/
		Mat recoverDes;
		postProcess(des, recoverDes);
		imwrite("test_gray.jpg", recoverDes);
		//matAnayzier logger(hiddenImage, recoverDes);
		//logger.generateLog(); 
		/**************************test**************/
		waitKey(0);
	}
}
void GCEmbedding::extracting() {
	hiddenImage = imread(hiddenImageName);
	secretMessege = new char[1000];
	srand(key);
	if (hiddenImage.channels() == 3) {
		converToYIQ(hiddenImage, hiddenImage);
		Mat des[3];
		Mat MChrom[3]; // 0 for luminance 1 for Yb 2 for Yr
		split(hiddenImage, MChrom);
		//preProcess(MChrom[0], des[0], luminance);
		//preProcess(MChrom[1], des[1], Chrominance);
		preProcess(MChrom[2], des[2], Chrominance);
		/*************cryptoblock  Start*************/
		messegeLength = 18; //要考虑获取信息长度的问题，这个是可以嵌入到信息中的，但是暂时不想考虑这个问题，这里直接赋值了
		_extracting(des[2], des[2], secretMessege, messegeLength);
		cout << secretMessege << endl;
		/*************cryptoblock  End*************/
		Mat recoverDes[3];
		recoverDes[0] = MChrom[0];
		recoverDes[1] = MChrom[1];
		//postProcess(des[0], recoverDes[0], luminance);
		//postProcess(des[1], recoverDes[1], Chrominance);
		postProcess(des[2], recoverDes[2], Chrominance);
		for (int i = 0; i < 3; i++)
			recoverDes[i] = Mat_<uchar>(recoverDes[i]);
		Mat result;
		mergeThreeBasicImgToColor(result, recoverDes[0], recoverDes[1], recoverDes[2]);
		cvtColor(result, result, CV_YCrCb2BGR);
	}
	else {
		Mat des;
		preProcess(hiddenImage, des);
		/*************cryptoblock  Start*************/
		messegeLength = 18; //要考虑获取信息长度的问题，这个是可以嵌入到信息中的，但是暂时不想考虑这个问题，这里直接赋值了
		_extracting(des, des, secretMessege, messegeLength);
		/*************cryptoblock  End*************/
		cout << secretMessege << endl;
		Mat recoverDes;
		postProcess(des, recoverDes);
		/**************************test**************/
	}
}
void GCEmbedding::PreNormalized(const Mat& imat,Mat& des) {
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	normalize(src, des, -128, 127, NORM_MINMAX);
	//des = des*255 - 128;
}
void GCEmbedding::PostNormalized(const Mat& imat, Mat& des) {
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	normalize(src, des, 0, 255, NORM_MINMAX);
}
void GCEmbedding::preProcess(const Mat& src, Mat& des , quantizer quanti) {
	des = Mat_<double>(src.size());
	CopyTo(src, des,int2double);
	PreNormalized(des, des);
	FDTC(des, des);
	Quantization(des, des, quanti);
}
void GCEmbedding::postProcess(const Mat& src, Mat& des, quantizer quanti) {
	des = Mat_<double>::zeros(src.size());
	src.copyTo(des);
	IQuantization(src, des, quanti);
	IFDTC(des, des);
	PostNormalized(des, des);
	CopyTo(des, des,double2int);
}
void GCEmbedding::converToYIQ(const Mat& imat,Mat& des) {
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	cvtColor(src, des, CV_BGR2YCrCb);
}
void GCEmbedding::Quantization(const Mat& imat, Mat &des, quantizer quanti) {
	assert(imat.size() == des.size());
	assert(imat.channels() == des.channels() == 1);
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	const int * q[8];
	switch (quanti)
	{
	case luminance:
		for(int i=0;i<8;i++)
			q[i] = lSQtable[i];
		break;
	case Chrominance:
		for (int i = 0; i<8; i++)
			q[i] = CSQtable[i];
		break;
	}
	for (int i = 0; i < src.rows - 8; i += 8) {
		for (int j = 0; j < src.cols - 8; j += 8) {
			Quantization(src(Range(i, i + 8), Range(j, j + 8)), des(Range(i, i + 8), Range(j, j + 8)),q);
		}
	}
}
void GCEmbedding::Quantization(Mat& src, Mat& des, const int* q[8]) {
	Size norm(8, 8);
	assert(norm == src.size() && norm == des.size());
	for (int i = 0; i < src.rows; i++) {
		double* s = src.ptr<double>(i);
		double* d = des.ptr<double>(i);
		for (int j = 0; j < src.cols; j++) {
			d[j] = round(s[j] / (q[i][j]));
		}
	}
}
void GCEmbedding::IQuantization(const Mat& imat, Mat &des, quantizer quanti) {
	assert(imat.size() == des.size());
	assert(imat.channels() == des.channels() == 1);
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	const int * q[8];
	switch (quanti)
	{
	case luminance:
		for (int i = 0; i<8; i++)
			q[i] = lSQtable[i];
		break;
	case Chrominance:
		for (int i = 0; i<8; i++)
			q[i] = CSQtable[i];
		break;
	}
	for (int i = 0; i < src.rows - 8; i += 8) {
		for (int j = 0; j < src.cols - 8; j += 8) {
			IQuantization(src(Range(i, i + 8), Range(j, j + 8)), des(Range(i, i + 8), Range(j, j + 8)), q);
		}
	}
}
void GCEmbedding::IQuantization(Mat& src, Mat& des, const int* q[8]) {
	Size norm(8, 8);
	assert(norm == src.size() && norm == des.size());
	for (int i = 0; i < src.rows; i++) {
		double* s = src.ptr<double>(i);
		double* d = des.ptr<double>(i);
		for (int j = 0; j < src.cols; j++) {
			d[j] = s[j] * (q[i][j]);
		}
	}
}
void main() {
	/*Mat a = imread("Img_test.jpg", 0);
	//Mat b = imread("test.jpg", 0);
	imwrite("Img.jpg", a);
	Mat b = imread("Img.jpg", 0);
	matAnayzier logger(a, b);
	logger.generateLog();*/
	GCEmbedding test("test.jpg",783);
	//GCEmbedding test("lena_color.jpg","test.txt", 783);
	
	//test.hidding();
	test.extracting();
}
void GCEmbedding::CopyTo(const Mat& imat, Mat&des,copyType type) {
	assert(imat.size() == des.size());
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	double *d;
	uint8_t * dd;
	switch (type) {
	case int2double:
		for (int i = 0; i < src.rows; i++) {
			d = des.ptr<double>(i);
			for (int j = 0; j < src.cols; j++)
				d[j] = src.at<uint8_t>(i, j);
		}
		break;
	case double2int:
		des = Mat_<uint8_t>(src.size());
		for (int i = 0; i < src.rows; i++) {
			dd = des.ptr<uint8_t>(i);
			for (int j = 0; j < src.cols; j++)
				dd[j] = round(src.at<double>(i, j));
		}
		break;
	}
	
}
void GCEmbedding::BDCT(Mat& src, Mat& des) {
	Size norm(8, 8);
	assert(norm == src.size() && norm == des.size());
	double ** y = new double*[8];
	double **Y = new double*[8];
	for (int i = 0; i < 8; i++) {
		y[i] = src.ptr<double>(i);
		Y[i] = des.ptr<double>(i);
	}
	for (int k = 0; k < 8; k++) {
		for (int l = 0; l < 8; l++) {
			int tmp = 0;
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					tmp = tmp + C(i, k)*C(j, l)*y[i][j];
				}
			}
			Y[k][l] = tmp / 4;
		}
	}
	delete[] y;
	delete[] Y;
}
void GCEmbedding::FDTC(const Mat& imat, Mat &des) {
	assert(imat.size() == des.size());
	assert(imat.channels() == des.channels() == 1);
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	for (int i = 0; i < src.rows-8; i+=8) {
		for (int j = 0; j < src.cols-8; j += 8) {
			BDCT(src(Range(i, i + 8), Range(j, j + 8)), des(Range(i, i + 8), Range(j, j + 8)));
		}
	}
}
void GCEmbedding::IBDCT(Mat& src, Mat& des) {
	Size norm(8, 8);
	assert(norm == src.size() && norm == des.size());
	double ** y = new double*[8];
	double **Y = new double*[8];
	for (int i = 0; i < 8; i++) {
		Y[i] = src.ptr<double>(i);
		y[i] = des.ptr<double>(i);
	}
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			int tmp = 0;
			for (int k = 0; k < 8; k++) {
				for (int l = 0; l < 8; l++) {
					tmp = tmp + C(i, k)*C(j, l)*Y[k][l];
				}
			}
			y[i][j] = tmp / 4;
		}
	}
	delete[] y;
	delete[] Y;
}
void GCEmbedding::IFDTC(const Mat& imat, Mat &des) {
	assert(imat.size() == des.size());
	assert(imat.channels() == des.channels() == 1);
	Mat src;
	src.copySize(imat);
	imat.copyTo(src);
	for (int i = 0; i < src.rows - 8; i += 8) {
		for (int j = 0; j < src.cols - 8; j += 8) {
			IBDCT(src(Range(i, i + 8), Range(j, j + 8)), des(Range(i, i + 8), Range(j, j + 8)));
		}
	}
}
void GCEmbedding::mergeThreeBasicImgToColor(Mat& OCImg, Mat& OBGImg, Mat& OGGImg, Mat& ORGImg) {
	std::vector<cv::Mat> array_to_merge;
	array_to_merge.push_back(OBGImg);
	array_to_merge.push_back(OGGImg);
	array_to_merge.push_back(ORGImg);
	cv::merge(array_to_merge, OCImg);
}
char* GCEmbedding::getMessageFlow(const string& fileName, unsigned int& mLength) {
	char* message;
	FILE * f = fopen(fileName.c_str(), "rb");
	unsigned long ulSize = 0;
	assert(!fseek(f, 0, SEEK_END)); /// move to end of file
	mLength = ftell(f);             /// calcute the size of file in byte
	assert(!fseek(f, 0, SEEK_SET));/// start to read message
	message = new char[mLength];
	fread(message, sizeof(char), mLength, f);
	fclose(f);                     /// close message file
	return message;
}