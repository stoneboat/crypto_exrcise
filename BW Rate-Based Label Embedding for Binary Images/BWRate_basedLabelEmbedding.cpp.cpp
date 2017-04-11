// CvWork.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <ctime>

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
	blockMap(const unsigned int _row_length,const unsigned int _col_length,Mat _blockSet) {
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
	void getNextBlockNum(unsigned int& rowN, unsigned int& colN);
	void diableBlock(unsigned int& rowN, unsigned int& colN);
private:
	Mat blockSet;
	unsigned int row_length;
	unsigned int col_length;
	bool** blockmap;
};
void blockMap::getNextBlockNum(unsigned int& row_num, unsigned int& col_num) {
	while (true) {
		row_num = rand() % row_length;
		col_num = rand() % col_length;
		if (!blockmap[row_num][col_num])
			continue;
		else
			break;
	}
}
void blockMap::diableBlock(unsigned int& rowN, unsigned int& colN) {
	blockmap[rowN][colN] = false;
}
class BWEmbedding {
public:
	void hidding(const string& resultFileName = "hiddenIMage.png");
	void extracting(const string& resultFileName = "extractMessege.txt");
	BWEmbedding(string _fileName, string _imageName,unsigned int _key) {
		key = _key;
		secreteFileName = _fileName;
		loadImageName = _imageName;
		c = getMessageFlow(_fileName, MessageLength);
		hiddenImage = imread(_imageName, 0);
		getHiddenImage(loadImageName, hiddenImage);
		imshow("tse", hiddenImage);
		blockManager = blockMap((hiddenImage.rows/8),(hiddenImage.cols/8),hiddenImage);
	}
	BWEmbedding(string _imageName, unsigned int _key) {
		key = _key;
		loadImageName = _imageName;
		MessageLength = 18;// just for specific example,can modify it later
		c = new char[MessageLength + 1];
		c[MessageLength] = '\0';
		hiddenImage = imread(_imageName, 0);
		getHiddenImage(loadImageName, hiddenImage);
		blockManager = blockMap((hiddenImage.rows / 8), (hiddenImage.cols / 8), hiddenImage);
	}
	~BWEmbedding() {
		if (c != NULL)
			delete[] c;
	}
	void getHiddenImage(const string& loadImageName, Mat& hidden);
private:
	enum blockType{dithered,highContrast};
	unsigned int key;
	const int ori[2] = { -1,1 };
	const double R0 = 29;
	const double R1 = 35;
	const double lam = 3;
	const int maxMesLength = 100;
	char* c; /// message flow
	Mat hiddenImage;
	blockMap blockManager;
	string secreteFileName;
	string loadImageName;
	bool checkWrite(const unsigned int & rn, const unsigned int& cn,unsigned int mes);
	bool checkRead(const unsigned int & rn, const unsigned int& cn);
	unsigned int MessageLength; // count in 8 bit unit
	char* getMessageFlow(const string& fileName, unsigned int&);
	void modifyBlock(const unsigned int& rn,const unsigned int& cn,const unsigned int modify_number, const unsigned int colortype);
	blockType checkBlockType(const unsigned int& rn, const unsigned int& cn);
	bool ifSame(const unsigned int r, const unsigned int c) {
		if (r == 0 || c == 0 || r == hiddenImage.rows-1 || c == hiddenImage.cols-1)
			return true;//边缘检测 太浪费了，直接返回true也没什么
		int tmp = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++) {
				if (hiddenImage.at<uint8_t>(ori[i]+r, ori[j]+c) != hiddenImage.at<uint8_t>(r, c))
					tmp++;
			}
		if (tmp > 1)
			return false;
		else
			return true;
	}
	void dModify(const unsigned int& rn, const unsigned int& cn,const unsigned int number,const unsigned int colortype);
	void cModify(const unsigned int& rn, const unsigned int& cn, const unsigned int number, const unsigned int colortype);
	unsigned int calculateOffset(const unsigned int& rn, const unsigned int& cn); //0-3
	void getBlackPixelRate(const unsigned int & rn, const unsigned int& cn, unsigned int& blackRatio) {
		blackRatio = 0;
		for (int i = rn * 8; i < rn * 8 + 8; i++) {
			uint8_t* f = hiddenImage.ptr<uint8_t>(i);
			for (int j = cn * 8; j < cn * 8 + 8; j++)
				if (f[j] == 0)
					blackRatio++;
		}
	}
};
void main() {
	BWEmbedding embed = BWEmbedding("test.txt","Img.png",783);
	//BWEmbedding embed = BWEmbedding("hiddenIMage.png", 783);
	//embed.hidding();
	//embed.extracting();
	cv::waitKey(0);
}
void BWEmbedding::extracting(const string& resultFileName ) {
	srand(key);
	unsigned int RblockNum = 0;
	unsigned int CblockNum = 0;
	unsigned int secretByte = 0;
	for (int i = 0; i < MessageLength; i++) {
		unsigned int tmp = 0;
		for (int j = 7; j >= 0; j--) {
			unsigned int blackRatio = 0;
			while (true) {
				blockManager.getNextBlockNum(RblockNum, CblockNum);
				if (checkRead(RblockNum, CblockNum)) {
					blockManager.diableBlock(RblockNum, CblockNum);
					break;
				}
				blockManager.diableBlock(RblockNum, CblockNum);
			}
			getBlackPixelRate(RblockNum, CblockNum, blackRatio);
			if (blackRatio > 32)
				secretByte = 1;
			if (blackRatio < 32)
				secretByte = 0;
			tmp = tmp + (secretByte << j);
		}
		c[i] = tmp;
	}
	ofstream file(resultFileName);
	file << c;
	file.close();
}
char* BWEmbedding::getMessageFlow(const string& fileName, unsigned int& mLength) {
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
	return NULL;
}
void BWEmbedding::getHiddenImage(const string& loadImageName, Mat& hiddenImage) {
	const int threshold = 128;
	for (int i = 0; i < hiddenImage.rows; i++) {
		uint8_t * f = hiddenImage.ptr<uint8_t>(i);
		for (int j = 0; j < hiddenImage.cols; j++) {
			if (f[j] > threshold)
				f[j] = 255;
			else
				f[j] = 0;
		}
	}
}
bool BWEmbedding::checkWrite(const unsigned int & rn, const unsigned int& cn, unsigned int mes) {
	unsigned int blackRatio = 0;
	getBlackPixelRate(rn, cn, blackRatio);
	if (blackRatio > R1 + 3 * lam || blackRatio < R0 - 3 * lam)
		return false;
	if (blackRatio > R1 && mes == 0) {
		modifyBlock(rn, cn, R1+3*lam-blackRatio,1);
		return false;
	}
	if (blackRatio < R0 && mes == 1) {
		modifyBlock(rn, cn, blackRatio -(R0-3*lam),0);
		return false;
	}
	return true;
}
bool BWEmbedding::checkRead(const unsigned int & rn, const unsigned int& cn) {
	unsigned int blackRatio = 0;
	getBlackPixelRate(rn, cn, blackRatio);
	if (blackRatio > R1 + 2 * lam || blackRatio < R0 - 2 * lam)
		return false;
	return true;
}
void BWEmbedding::hidding(const string& resultFileName) {
	srand(key);
	unsigned int RblockNum = 0;
	unsigned int CblockNum = 0;
	unsigned int secretByte = 0;
	for (int i = 0; i < MessageLength; i++) {
		unsigned int tmp = c[i];
		for (int j = 7; j >=0; j--) {
			unsigned int blackRatio = 0;
			secretByte = (tmp >> j);
			tmp = tmp - (secretByte << j);
			while (true) {
				blockManager.getNextBlockNum(RblockNum, CblockNum);
				if (checkWrite(RblockNum, CblockNum, secretByte)) {
					blockManager.diableBlock(RblockNum, CblockNum);
					break;
				}
				blockManager.diableBlock(RblockNum, CblockNum);
			}
			getBlackPixelRate(RblockNum, CblockNum, blackRatio);
			if (secretByte == 1) {
				unsigned int modify = 0;
				if (blackRatio < R1)
					modifyBlock(RblockNum, CblockNum, R1 - blackRatio, 1);
				if(blackRatio>R1+lam)
					modifyBlock(RblockNum, CblockNum, blackRatio-R1-lam, 0);
			}
			if (secretByte == 0) {
				unsigned int modify = 0;
				if (blackRatio > R0)
					modifyBlock(RblockNum, CblockNum, blackRatio -R0, 0);
				if (blackRatio<R0 - lam)
					modifyBlock(RblockNum, CblockNum, R0 - lam -blackRatio, 1);
			}
		}
	}
	imwrite(resultFileName, hiddenImage);
}
void BWEmbedding::modifyBlock(const unsigned int& rn, const unsigned int& cn, const unsigned int modify_number, const unsigned int colortype) {
	blockType btype = checkBlockType(rn, cn);
	if (btype == blockType::dithered) {
		dModify(rn, cn, modify_number + calculateOffset(rn, cn), colortype);
	}
	if (btype == blockType::highContrast) {
		dModify(rn, cn, modify_number + calculateOffset(rn, cn), colortype);
	}
}
unsigned int BWEmbedding::calculateOffset(const unsigned int& rn, const unsigned int& cn) {
	return (rn + cn) % 4;
}
BWEmbedding::blockType BWEmbedding::checkBlockType(const unsigned int& rn, const unsigned int& cn) {
	unsigned int dFeature = 0;
	// too ungly , I should use vector...
	for (int i = rn * 8; i < rn * 8 + 8; i++) {
		for (int j = cn * 8; j < cn * 8 + 8; j++) {
			if (!ifSame(i,j))
				dFeature++;
		}
	}
	if (dFeature > 20)
		return blockType::dithered;
	else
		return blockType::highContrast;
}
void BWEmbedding::dModify(const unsigned int& rn, const unsigned int& cn, const unsigned int numb, const unsigned int colortype) {
	unsigned int type = 255 - colortype * 255;
	int number = numb;
	if (number <= 0)
		return;
	for (int i = rn * 8; i < rn * 8 + 8; i++) {
		for(int j=cn*8;j<cn*8+8;j++){
			if (hiddenImage.at<uint8_t>(i, j) != type&&ifSame(i, j)) {
				hiddenImage.at<uint8_t>(i, j) = type;
				number--;
				if (number == 0)
					return;
			}
		}
	}
}
void BWEmbedding::cModify(const unsigned int& rn, const unsigned int& cn, const unsigned int numb, const unsigned int colortype) {
	unsigned int type = 255 - colortype * 255;
	int number = numb;
	if (number <= 0)
		return;
	for (int i = rn * 8; i < rn * 8 + 8; i++) {
		for (int j = cn * 8; j<cn * 8 + 8; j++) {
			if (hiddenImage.at<uint8_t>(i, j) != type&&(!ifSame(i, j))) {
				hiddenImage.at<uint8_t>(i, j) = type;
				number--;
				if (number == 0)
					return;
			}
		}
	}
}
