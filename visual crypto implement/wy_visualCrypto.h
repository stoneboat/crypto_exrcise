#pragma once
#include "stdafx.h"
#include <ctime>
/*
@author stoneboat
@function 
		The namespace contains components implementing one kind of old basic method to make visual crypto on original pic
	Components can divide to three parts.
		Part 1 contains the functionality methods. The most basic one is to generate two black and white meaningless pictures for a black and white picture(This part lost, just implement one mapping rule can do it). Then is for gray scale(one channel), color scale(three channel) picture. Strictly speaking， the latter two can not be sawn as visual crypto. I just simply hide the pixels' information into two different meaningless pictures.
	Each pixel value use this:  key1 + key2 = original mod 256.	The other branch hides original piture into two innocent looking pictures, obeying the principle that making contrast between black sub pixels and white sub pixels. I just do like [NaorShamir-VisualCryptography] says. In order to using the same principle in gray and color pictures, we use the halftoning technique to make gray scale value(0-255) to black(0) and white(255) only.
	I heard from the website that comparing to iterating and averaging the error diffusion is better and more efficient. So the work backing to the start, it's all the same.
		Part 2 contains some utility which provide for supporting functionality methods. It contains the subPixel groups relating to shares for C0 and C1 which is mentioned in the paper written by Shamir referenced above. It also contains the operation for matrix like copying line and copying matrix, I should make them in a class for matrix.
	It contains the mapping rules for distributing random share for key subgroups in stacking method. For example, black pixel(original) = white pixel(key1) + black pxiel(key2). How it maps? I store in  a structure. It also contains some patches for the tranforming form matlab to there.
	I should admit that my codes is totally a bucket of rubbish. I test using matlab first and transfer most codes here to make them have hierachical structure and friendly to use. I know I fail almostly ,however, I lost interest and there are many other more important things I should do.
		Part 3 it's the api to call. I'll test them in simleVC_test.

*/
namespace WY_VC1 {
	//api
	void genGrayKeyPic(string img); //visual crypto of gray pic not depending on innocent looking key
	void recoverGrayPic(string key1, string key2); //visual crypto of color pic not depending on innocent looking key

	void genColorKeyPic(string imgName);//generating of visual crypto of color pic not depending on innocent looking key
	void recoverColorKeyPic(string OBGkey1, string OBGkey2, string OGGkey1, string OGGkey2, string ORGkey1, string ORGkey2);//recovering of visual crypto of color pic not depending on innocent looking key

	void genBWStackingKeyPic(string imgName, string key1Name, string key2Name);
	Mat recoverBWStackingPic(string key1Name, string key2Name, string filename);

	Mat recoverGrayStackingPic(string key1Name, string key2Name, string filename);
	Mat recoverColorStackingPic(string key1Name, string key2Name, string filename = "color_recover");

	void genGrayStackingKeyPic(string imgName, string key1Name, string key2Name);
	void genColorStackingKeyPic(string imgName, string key1Name, string key2Name);

	//utility
	class subPixelGC { //subPixelGroup Class
	public:
		uint groupSize;
		uint matrix_Rows;
		uint matrix_Cols;
		bool*** data;
		static enum MatrixType { Normal, White, Black };
		subPixelGC(uint size, uint rows, uint cols, MatrixType type) {
			groupSize = size;
			matrix_Rows = rows;
			matrix_Cols = cols;
			data = genBWsubPixelGroup();
			switch (type) {
			case Normal:
				generateNormalG();
				break;
			case White:
				generateNormalG();
				turn2SubWhite();
				break;
			case Black:
				turn2SubBlack();
				break;
			}

			//check();
		}

		~subPixelGC() {
			constructBWsubPixelGroup(data);
		}
		void check() {
			for (int i = 1; i <= groupSize; i++) {
				for (int r = 1; r <= matrix_Rows; r++) {
					for (int c = 1; c <= matrix_Cols; c++)
						cout << data[i][r][c] << " ";
					cout << endl;
				}
				cout << endl;
			}
		}
	private:
		void generateNormalG() {
			for (int i = 1; i <= 2; i++) {
				copyRow(data[i][1], lineG[i]);
				copyRow(data[i][2], lineG[5 - i]);
				copyRow(data[i + 2][1], lineG[i + 1]);
				copyRow(data[i + 2][2], lineG[i + 1]);
				copyRow(data[7 - i][1], lineG[5 - i]);
				copyRow(data[7 - i][2], lineG[i]);
			}
		}
		void turn2SubWhite() {
			bool*** backup = genBWsubPixelGroup();
			copyMatrix(backup[1], data[4]);
			copyMatrix(backup[2], data[6]);
			copyMatrix(backup[3], data[5]);
			copyMatrix(backup[4], data[2]);
			copyMatrix(backup[5], data[1]);
			copyMatrix(backup[6], data[3]);
			constructBWsubPixelGroup(data);
			data = backup;
		}
		void turn2SubBlack() {
			for (int i = 1; i <= groupSize; i++) {
				for (int r = 1; r <= matrix_Rows; r++) {
					for (int c = 1; c <= matrix_Cols; c++)
						data[i][r][c] = 1;
				}
			}
			data[1][1][1] = 0;
			data[2][1][2] = 0;
			data[3][2][2] = 0;
			data[4][2][1] = 0;
		}
		void copyRow(bool* des, bool* src) {
			for (int i = 1; i <= 2; i++)
				des[i] = src[i];
		}
		void copyMatrix(bool** des, bool** src) {
			for (int r = 1; r <= matrix_Rows; r++) {
				for (int c = 1; c <= matrix_Cols; c++)
					des[r][c] = src[r][c];
			}
		}
		void constructBWsubPixelGroup(bool*** data) {
			for (int i = 1; i <= groupSize; i++) {
				for (int j = 1; j <= matrix_Rows; j++)
					delete[] data[i][j];
				delete[] data[i];
			}
			delete[] data;
		}
		bool*** genBWsubPixelGroup() {
			bool*** data = new bool**[groupSize + 1];
			for (int i = 1; i <= groupSize; i++) {
				data[i] = new bool*[matrix_Rows + 1];
				for (int j = 1; j <= matrix_Rows; j++)
					data[i][j] = new bool[matrix_Cols + 1];
			}
			return data;
		}
		bool lineG[5][3] = { 0,0,0,
			0,1,1,
			0,0,1,
			0,1,0,
			0,0,0 };

	};
	class subPixelMapping {
	public:
		uint** data;
		uint groupSize;
		uint pixelKind;
		static enum MappingType { White, Black };
		subPixelMapping(uint size, MappingType type) {
			groupSize = size;
			pixelKind = 2;
			data = new uint*[groupSize + 1];
			for (int i = 1; i <= groupSize; i++) {
				data[i] = new uint[pixelKind + 1];
			}
			if (type == White)
				turn2WhiteMapping();
			if (type == Black)
				turn2BlackMapping();
		}
		~subPixelMapping() {
			for (int i = 1; i <= groupSize; i++) {
				delete[] data[i];
			}
			delete[] data;
		}
	private:
		void turn2WhiteMapping() {
			data[1][1] = 2; data[1][2] = 3;
			data[2][1] = 1; data[2][2] = 2;
			data[3][1] = 2; data[3][2] = 4;
			data[4][1] = 1; data[4][2] = 3;
			data[5][1] = 3; data[5][2] = 4;
			data[6][1] = 1; data[6][2] = 4;
		}
		void turn2BlackMapping() {
			data[1][1] = 1; data[1][2] = 4;
			data[2][1] = 3; data[2][2] = 4;
			data[3][1] = 1; data[3][2] = 3;
			data[4][1] = 2; data[4][2] = 4;
			data[5][1] = 1; data[5][2] = 2;
			data[6][1] = 2; data[6][2] = 3;
		}
	};
	const int Const_gray = 256;
	uint globalMap[2][2][2] = { 2,6,7,0,3,4,5,1 };
	uchar patch4copySubMatrix(bool x);
	bool isDiff(Mat& des, bool** src, int i, int j);
	string getABSpath(string path);
	string getPicType(string path);
	string genKeyName(string path, string keyName);
	string backUpPath(string path);
	void getRTag(subPixelGC& SubBlack, subPixelGC& SubWhite, Mat&des, int i, int j, bool& color, int& index);
	void copySubMatrix(Mat& des, bool** src, int i, int j);
	int  getSubWhiteIndex(subPixelGC& WhiteBlack, Mat&des, int i, int j);
	int  getSubBlackIndex(subPixelGC& SubBlack, Mat&des, int i, int j);

	// functionality
	void mergeThreeBasicImgToColor(Mat& OCImg, Mat& OBGImg, Mat& OGGImg, Mat& ORGImg) {
		std::vector<cv::Mat> array_to_merge;
		array_to_merge.push_back(OBGImg);
		array_to_merge.push_back(OGGImg);
		array_to_merge.push_back(ORGImg);
		cv::merge(array_to_merge, OCImg);
	}
	void Error_Diffusion_Halftoning(Mat GImg, Mat& BWImg); //Error_Diffusion_Halftoning of gray pic
	void genGrayKeyPic(Mat& OGImg, Mat& Key1_Img, Mat& Key2_Img);//generating of visual crypto of gray pic not depending on innocent looking key
	void recoverGrayKeyPic(Mat& OGImg, Mat& Key1_Img, Mat& Key2_Img);//recovering of visual crypto of gray pic not depending on innocent looking key
	void spiltColorToThreeBasicImg(Mat& OCImg, Mat& OBGImg, Mat& OGGImg, Mat& ORGImg);//spiliting color image to B,G,R separating gray image
	void genBWStackingKeyPic(Mat& bwO, Mat& bwK1, Mat& bwK2, Mat& SK1, Mat& SK2);//visual crypto of black and white pic depending on innocent looking key ,reference the original pic and two innocent pic to generate Key pic SK1 and SK2
	void recoverBWStackingPic(Mat& bwO_recover, Mat& SK1, Mat& SK2);
	void genGrayStackingKeyPic(string path, Mat& OGImg, Mat& OGK1, Mat& OGK2, Mat& SK1, Mat& SK2, string fileName = "Gray");//visual crypto of gray pic depending on innocent looking key ,reference the original pic and two innocent pic to generate Key pic SK1 and SK2
	void genColorStackingKeyPic(string path, Mat& OCImg, Mat& OCK1, Mat& OCK2, Mat& SK1, Mat& SK2);//visual crypto of color pic depending on innocent looking key ,reference the original pic and two innocent pic to generate Key pic SK1 and SK2

	
	//free code for checking
	void check();

	// implementation
	void genColorStackingKeyPic(string imgName, string key1Name, string key2Name) {
		srand((unsigned)time(0));
		string absPath = getABSpath(imgName);
		Mat OCImg = imread(absPath, 1);
		Mat OCK1 = imread(getABSpath(key1Name), 1);
		Mat OCK2 = imread(getABSpath(key2Name), 1);
		Mat SK1(OCImg.rows * 2, OCImg.cols * 2, CV_8UC(3), Scalar::all(0));
		Mat SK2(OCImg.rows * 2, OCImg.cols * 2, CV_8UC(3), Scalar::all(0));
		genColorStackingKeyPic(absPath, OCImg, OCK1, OCK2, SK1, SK2);
		imwrite(genKeyName(absPath, "Colorkey1"), SK1);
		imwrite(genKeyName(absPath, "Colorkey2"), SK2);
	}
	void genGrayStackingKeyPic(string imgName, string key1Name, string key2Name) {
		srand((unsigned)time(0));
		string absPath = getABSpath(imgName);
		Mat OGImg = imread(absPath, 0);
		Mat OGK1 = imread(getABSpath(key1Name), 0);
		Mat OGK2 = imread(getABSpath(key2Name), 0);
		Mat SK1(OGImg.rows * 2, OGImg.cols * 2, CV_8UC(1), Scalar::all(0));
		Mat SK2(OGImg.rows * 2, OGImg.cols * 2, CV_8UC(1), Scalar::all(0));
		genGrayStackingKeyPic(imgName,OGImg, OGK1, OGK2, SK1, SK2);
		imwrite(genKeyName(absPath, "Graykey1"), SK1);
		imwrite(genKeyName(absPath, "Graykey2"), SK2);
	}
	void genBWStackingKeyPic(string imgName, string key1Name, string key2Name) {
		srand((unsigned)time(0));
		string absPath = getABSpath(imgName);
		Mat OGImg = imread(absPath,0);
		Mat OBWK1 = imread(getABSpath(key1Name), 0);
		Mat OBWK2 = imread(getABSpath(key2Name), 0);
		threshold(OGImg, OGImg, 128, 255, THRESH_BINARY);
		threshold(OBWK1, OBWK1, 128, 255, THRESH_BINARY);
		threshold(OBWK2, OBWK2, 128, 255, THRESH_BINARY);
		Mat SK1(OGImg.rows*2, OGImg.cols*2, CV_8UC(1), Scalar::all(0));
		Mat SK2(OGImg.rows*2, OGImg.cols*2, CV_8UC(1), Scalar::all(0));
		genBWStackingKeyPic(OGImg, OBWK1, OBWK2, SK1, SK2);
		imwrite(genKeyName(absPath, "BWkey1"), SK1);
		imwrite(genKeyName(absPath, "BWkey2"), SK2);
	}
	Mat recoverBWStackingPic(string key1Name, string key2Name,string filename= "BW_recover") {
		Mat OBWK1 = imread(getABSpath(key1Name), 0);
		Mat OBWK2 = imread(getABSpath(key2Name), 0);
		Mat OGRecoverIMG(OBWK1.rows / 2, OBWK1.cols / 2, CV_8UC(1), Scalar::all(0));
		recoverBWStackingPic(OGRecoverIMG, OBWK1, OBWK2);
		imwrite(genKeyName(getABSpath(key2Name), filename), OGRecoverIMG);
		return OGRecoverIMG;
	}
	Mat recoverGrayStackingPic(string key1Name, string key2Name, string filename = "Gray_recover") {
		return recoverBWStackingPic(key1Name, key2Name, filename);
	}
	Mat recoverColorStackingPic(string key1Name, string key2Name, string filename) {
		// in this part,the redundant works can be replaced by macro definition
		Mat OCK1 = imread(getABSpath(key1Name), 1);
		Mat OBK1Img(OCK1.rows, OCK1.cols, CV_8UC(1), Scalar::all(0));
		Mat OGK1Img(OCK1.rows, OCK1.cols, CV_8UC(1), Scalar::all(0));
		Mat ORK1Img(OCK1.rows, OCK1.cols, CV_8UC(1), Scalar::all(0));
		spiltColorToThreeBasicImg(OCK1, OBK1Img, OGK1Img, ORK1Img);
		string OBK1_Path = genKeyName(getABSpath(key1Name), "OBK1_recover");
		string OGK1_Path = genKeyName(getABSpath(key1Name), "OGK1_recover");
		string ORK1_Path = genKeyName(getABSpath(key1Name), "ORK1_recover");
		imwrite(OBK1_Path, OBK1Img);
		imwrite(OGK1_Path, OGK1Img);
		imwrite(ORK1_Path, ORK1Img);
		Mat OCK2 = imread(getABSpath(key2Name), 1);
		Mat OBK2Img(OCK2.rows, OCK2.cols, CV_8UC(1), Scalar::all(0));
		Mat OGK2Img(OCK2.rows, OCK2.cols, CV_8UC(1), Scalar::all(0));
		Mat ORK2Img(OCK2.rows, OCK2.cols, CV_8UC(1), Scalar::all(0));
		spiltColorToThreeBasicImg(OCK2, OBK2Img, OGK2Img, ORK2Img);
		string OBK2_Path = genKeyName(getABSpath(key1Name), "OBK2_recover");
		string OGK2_Path = genKeyName(getABSpath(key1Name), "OGK2_recover");
		string ORK2_Path = genKeyName(getABSpath(key1Name), "ORK2_recover");
		imwrite(OBK2_Path, OBK2Img);
		imwrite(OGK2_Path, OGK2Img);
		imwrite(ORK2_Path, ORK2Img);
		string OB_Path = genKeyName(getABSpath(key1Name), "OB_recover");
		Mat OBImg_recover = recoverGrayStackingPic(OBK1_Path, OBK2_Path, "OB_recover");
		Mat OGImg_recover = recoverGrayStackingPic(OGK1_Path, OGK2_Path, "OG_recover");
		Mat ORImg_recover = recoverGrayStackingPic(ORK1_Path, ORK2_Path, "OR_recover");
		Mat color(OCK1.rows/2, OCK1.cols/2, CV_8UC(3), Scalar::all(0));
		mergeThreeBasicImgToColor(color, OBImg_recover, OGImg_recover, ORImg_recover);
		imwrite(genKeyName(getABSpath(key2Name), filename), color);
		return color;
	}
	void check() {
		Mat OGImg = imread("../OGImg.png");
		cvtColor(OGImg, OGImg, CV_BGR2GRAY);
		Mat OGI2BW_Img(OGImg.rows, OGImg.cols, CV_8UC(1), Scalar::all(0));
		Error_Diffusion_Halftoning(OGImg, OGI2BW_Img);
		imshow("Display window", OGI2BW_Img);
		int a = 4;
	}
	void genGrayKeyPic(string imgName) {
		srand((unsigned)time(0));
		string absPath = getABSpath(imgName);
		Mat OriginCImg = imread(absPath); // read color img
		Mat OGImg; // convert it to gray
		cvtColor(OriginCImg, OGImg, CV_BGR2GRAY);
		Mat Key1_Img(OGImg.rows, OGImg.cols, CV_8UC(1), Scalar::all(0));
		Mat Key2_Img(OGImg.rows, OGImg.cols, CV_8UC(1), Scalar::all(0));
		genGrayKeyPic(OGImg, Key1_Img, Key2_Img);
		imwrite(genKeyName(absPath,"key1"), Key1_Img);
		imwrite(genKeyName(absPath, "key2"), Key2_Img);
		imwrite(genKeyName(absPath, "OGImg"), OGImg);
	}
	void recoverGrayPic(string key1, string key2) {
		Mat Key1_Img = imread(getABSpath(key1), 0);
		Mat Key2_Img = imread(getABSpath(key2), 0);
		Mat OGImg(Key1_Img.rows, Key1_Img.cols, CV_8UC(1), Scalar::all(0)); // to recover origin gray pic
		recoverGrayKeyPic(OGImg, Key1_Img, Key2_Img);
		imwrite(genKeyName(getABSpath(key1), "OGImg_recover"), OGImg);
	}
	void genGrayKeyPic(Mat& OGImg, Mat& Key1_Img, Mat& Key2_Img) {
		uint r = 0;//relative distance
		uint a = 0;// pixel value of key1
		uint b = 0;// pixel value of key2
				   // satisfy a+b = r mod 256 , the return use the same fomula
		for (int x = 0; x<OGImg.rows; x++)
		{
			for (int y = 0; y<OGImg.cols; y++)
			{
				r = OGImg.at<uchar>(x, y);
				a = rand() % Const_gray;
				b = (Const_gray + r - a) % Const_gray;
				Key1_Img.at<uchar>(x, y) = a;
				Key2_Img.at<uchar>(x, y) = b;
				//check model
				uint rr = (a + b) % Const_gray;
				if (r != rr) {
					string x = "please stop here";
				}
			}
		}
	}
	void recoverGrayKeyPic(Mat& OGImg, Mat& Key1_Img, Mat& Key2_Img) {
		srand((unsigned)time(0));
		uint r = 0;//relative distance
		uint a = 0;// pixel value of key1
		uint b = 0;// pixel value of key2
				   // satisfy a+b = r mod 256 , the return use the same fomula
		for (int x = 0; x<OGImg.rows; x++)
		{
			for (int y = 0; y<OGImg.cols; y++)
			{
				a = Key1_Img.at<uchar>(x, y);
				b = Key2_Img.at<uchar>(x, y);
				r = (a + b) % Const_gray;
				OGImg.at<uchar>(x, y) = r;
			}
		}
	}
	void recoverGrayKeyPic_test(Mat& OGImg, Mat& OGImg_ori, Mat& Key1_Img, Mat& Key2_Img) {
		srand((unsigned)time(0));
		uint r = 0;//relative distance
		uint a = 0;// pixel value of key1
		uint b = 0;// pixel value of key2
				   // satisfy a+b = r mod 256 , the return use the same fomula
		for (int x = 0; x<OGImg.rows; x++)
		{
			for (int y = 0; y<OGImg.cols; y++)
			{
				a = Key1_Img.at<uchar>(x, y);
				b = Key2_Img.at<uchar>(x, y);
				r = (a + b) % Const_gray;
				OGImg.at<uchar>(x, y) = r;
				uint rr = OGImg_ori.at<uchar>(x, y);
			}
		}
	}
	void genColorKeyPic(string imgName) {
		srand((unsigned)time(0));
		string absPath = getABSpath(imgName);
		Mat OCImg = imread(absPath, 1); // read color img
		Mat OBGImg(OCImg.rows, OCImg.cols, CV_8UC(1), Scalar::all(0));
		Mat OGGImg(OCImg.rows, OCImg.cols, CV_8UC(1), Scalar::all(0));
		Mat ORGImg(OCImg.rows, OCImg.cols, CV_8UC(1), Scalar::all(0));
		spiltColorToThreeBasicImg(OCImg, OBGImg, OGGImg, ORGImg);
		Mat Key1_OBGImg(OBGImg.rows, OBGImg.cols, CV_8UC(1), Scalar::all(0));
		Mat Key2_OBGImg(OBGImg.rows, OBGImg.cols, CV_8UC(1), Scalar::all(0));
		Mat Key1_OGGImg(OGGImg.rows, OGGImg.cols, CV_8UC(1), Scalar::all(0));
		Mat Key2_OGGImg(OGGImg.rows, OGGImg.cols, CV_8UC(1), Scalar::all(0));
		Mat Key1_ORGImg(ORGImg.rows, ORGImg.cols, CV_8UC(1), Scalar::all(0));
		Mat Key2_ORGImg(ORGImg.rows, ORGImg.cols, CV_8UC(1), Scalar::all(0));
		genGrayKeyPic(OBGImg, Key1_OBGImg, Key2_OBGImg);
		genGrayKeyPic(OGGImg, Key1_OGGImg, Key2_OGGImg);
		genGrayKeyPic(ORGImg, Key1_ORGImg, Key2_ORGImg);
		imwrite(genKeyName(absPath, "OBGImg"), OBGImg);
		imwrite(genKeyName(absPath, "OGGImg"), OGGImg);
		imwrite(genKeyName(absPath, "ORGImg"), ORGImg);
		imwrite(genKeyName(absPath, "Key1_OBGImg"), Key1_OBGImg);
		imwrite(genKeyName(absPath, "Key2_OGGImg"), Key2_OGGImg);
		imwrite(genKeyName(absPath, "Key1_ORGImg"), Key1_ORGImg);
		imwrite(genKeyName(absPath, "Key2_OBGImg"), Key2_OBGImg);
		imwrite(genKeyName(absPath, "Key1_OGGImg"), Key1_OGGImg);
		imwrite(genKeyName(absPath, "Key2_ORGImg"), Key2_ORGImg);
	}
	void recoverColorKeyPic(string OBGkey1, string OBGkey2, string OGGkey1, string OGGkey2, string ORGkey1, string ORGkey2)
	{
		Mat Key1_OBGImg = imread(getABSpath(OBGkey1), 0);
		Mat Key2_OBGImg = imread(getABSpath(OBGkey2), 0);
		Mat OBGImg(Key1_OBGImg.rows, Key1_OBGImg.cols, CV_8UC(1), Scalar::all(0));
		recoverGrayKeyPic(OBGImg, Key1_OBGImg, Key2_OBGImg);
		Mat Key1_OGGImg = imread(getABSpath(OGGkey1), 0);
		Mat Key2_OGGImg = imread(getABSpath(OGGkey2), 0);
		Mat OGGImg(Key1_OGGImg.rows, Key1_OGGImg.cols, CV_8UC(1), Scalar::all(0));
		recoverGrayKeyPic(OGGImg, Key1_OGGImg, Key2_OGGImg);
		Mat Key1_ORGImg = imread(getABSpath(ORGkey1), 0);
		Mat Key2_ORGImg = imread(getABSpath(ORGkey2), 0);
		Mat ORGImg(Key1_ORGImg.rows, Key1_ORGImg.cols, CV_8UC(1), Scalar::all(0));
		recoverGrayKeyPic(ORGImg, Key1_ORGImg, Key2_ORGImg);
		imwrite(genKeyName(getABSpath(OBGkey1), "OBGImg_recover"), OBGImg);
		imwrite(genKeyName(getABSpath(OGGkey1), "OGGImg_recover"), OGGImg);
		imwrite(genKeyName(getABSpath(ORGkey1), "ORGImg_recover"), ORGImg);
		Mat color;
		mergeThreeBasicImgToColor(color, OBGImg, OGGImg, ORGImg);
		imwrite(genKeyName(getABSpath(OBGkey1), "OCImg_recover"), color);
	}
	void spiltColorToThreeBasicImg(Mat& OCImg, Mat& OBGImg, Mat& OGGImg, Mat& ORGImg) {
		for (int x = 0; x<OCImg.rows; x++)
		{
			for (int y = 0; y<OCImg.cols; y++)
			{
				Vec3b intensity = OCImg.at<Vec3b>(x, y);
				OBGImg.at<uchar>(x, y) = intensity.val[0];
				OGGImg.at<uchar>(x, y) = intensity.val[1];
				ORGImg.at<uchar>(x, y) = intensity.val[2];
			}
		}
	}
	// input an gray image , halftoning it and return an black and white image
	void Error_Diffusion_Halftoning(Mat GImg_Ori, Mat& BWImg) {
		Mat GImg = GImg_Ori.clone();
		//initial err arrays
		uchar** err = new uchar*[GImg.rows];
		for (int i = 0; i < GImg.rows; i++)
			err[i] = new uchar[GImg.cols];
		for (int i = 0; i < GImg.rows; i++)
			for (int j = 0; j < GImg.cols; j++)
				err[i][j] = 0;
		uchar diff = 0;
		for (int x = 0; x < GImg.cols; x++) {
			for (int y = 0; y < GImg.rows; y++) {
				// the threshold default is 128, while on presenting the pic based on low grey, it displays very bad , I change it to 256 , feel better more, maybe I will research it later
				// picture rendering in shallow gray scaler need high threshold while picture has apprent contrast need median threshold may I need a check of picture to determined what threshold to set to it
				if ((GImg.at<uchar>(y, x) + err[y][x])<255)
					BWImg.at<uchar>(y, x) = 0;
				else BWImg.at<uchar>(y, x) = 255;
				// this halftoning matrix can be changed basic on the need
				// how to construct it, Maybe later I can try to find it
				diff = (GImg.at<uchar>(y, x) + err[y][x]) - BWImg.at<uchar>(y, x);
				if (x<(GImg.cols - 1))
					err[y][x + 1] = err[y][x + 1] + diff * 7 / 16;
				if (y < (GImg.rows - 1)) {
					err[y + 1][x] = err[y + 1][x] + diff * 5 / 16;
					if (x>1)
						err[y + 1][x - 1] = err[y + 1][x - 1] + diff * 3 / 16;
					if (x<(GImg.cols - 1))
						err[y + 1][x + 1] = err[y + 1][x + 1] + diff * 1 / 16;
				}
			}
		}
		//delete err arrays
		for (int i = 0; i < GImg.rows; i++)
			delete[] err[i];
		delete[] err;
	}
	void getABC(uchar ori, uchar key1, uchar key2, uchar& a, uchar& b, uchar& c) {
		if (ori == 255)
			a = 0; else a = 1;
		if (key1 == 255)
			b = 0; else b = 1;
		if (key2 == 255)
			c = 0; else c = 1;
	}
	void genBWStackingKeyPic(Mat& bwO, Mat& bwK1, Mat& bwK2, Mat& SK1, Mat& SK2) {
		subPixelGC SubWhite(6, 2, 2, subPixelGC::White);
		subPixelGC SubBlack(4, 2, 2, subPixelGC::Black);
		subPixelMapping WB2BMap(6, subPixelMapping::Black);
		subPixelMapping WB2WMap(6, subPixelMapping::White);
		uchar a = 0;
		uchar b = 0;
		uchar c = 0;
		for (int i = 0; i < bwO.rows; i++) {
			for (int j = 0; j < bwO.cols; j++) {
				uint num = 0;
				uint n1 = 0;
				uint n2 = 0;
				getABC(bwO.at<uchar>(i, j), bwK1.at<uchar>(i, j), bwK2.at<uchar>(i, j), a, b, c); //为了直接翻译MATLAB代码，写的垃圾
				switch (globalMap[a][b][c]) {
				case 0:
					num = rand() % 4+1;
					copySubMatrix(SK1, SubBlack.data[num], i, j);
					copySubMatrix(SK2, SubBlack.data[num], i, j);
					break;
				case 1:
					n1 = rand() % 4 + 1;
					n2 = rand() % 4 + 1;
					while (n1 == n2)
						n2 = rand() % 4 + 1;
					copySubMatrix(SK1, SubBlack.data[n1], i, j);
					copySubMatrix(SK2, SubBlack.data[n2], i, j);
					break;
				case 2:
					n1 = rand() % 6 + 1;
					n2 = rand() % 6 + 1;
					while ((n1 + n2) == 7)
						n2 = rand() % 6 + 1;
					copySubMatrix(SK1, SubWhite.data[n1], i, j);
					copySubMatrix(SK2, SubWhite.data[n1], i, j);
					break;
				case 3:
					num = rand() % 6 + 1;
					copySubMatrix(SK1, SubWhite.data[num], i, j);
					copySubMatrix(SK2, SubWhite.data[7-num], i, j);
					break;
				case 4:
					n1 = rand() % 6 + 1;
					n2 = rand() % 2 + 1;
					copySubMatrix(SK1, SubWhite.data[n1], i, j);
					copySubMatrix(SK2, SubBlack.data[WB2BMap.data[n1][n2]], i, j);
					break;
				case 5:
					n1 = rand() % 6 + 1;
					n2 = rand() % 2 + 1;
					copySubMatrix(SK2, SubWhite.data[n1], i, j);
					copySubMatrix(SK1, SubBlack.data[WB2BMap.data[n1][n2]], i, j);
					break;
				case 6:
					n1 = rand() % 6 + 1;
					n2 = rand() % 2 + 1;
					copySubMatrix(SK1, SubWhite.data[n1], i, j);
					copySubMatrix(SK2, SubBlack.data[WB2WMap.data[n1][n2]], i, j);
					break;
				case 7:
					n1 = rand() % 6 + 1;
					n2 = rand() % 2 + 1;	
					copySubMatrix(SK2, SubWhite.data[n1], i, j);
					copySubMatrix(SK1, SubBlack.data[WB2WMap.data[n1][n2]], i, j);
					break;
				}
			}

		}
			
	}
	void genGrayStackingKeyPic(string path,Mat& OGImg, Mat& OGK1, Mat& OGK2, Mat& SK1, Mat& SK2,string fileName) {
		Error_Diffusion_Halftoning(OGImg, OGImg);
		Error_Diffusion_Halftoning(OGK1, OGK1);
		Error_Diffusion_Halftoning(OGK2, OGK2);
		imwrite(genKeyName(getABSpath(path), fileName+"Img_BW"), OGImg);
		imwrite(genKeyName(getABSpath(path), fileName+"K1_BW"), OGK1);
		imwrite(genKeyName(getABSpath(path), fileName+"K2_BW"), OGK2);
		genBWStackingKeyPic(OGImg, OGK1, OGK2, SK1, SK2);
	}
	void genColorStackingKeyPic(string path, Mat& OCImg, Mat& OCK1, Mat& OCK2, Mat& SK1, Mat& SK2) {
		// spilit color image to RGB three Gray scaler images
		Mat OBGImg(OCImg.rows, OCImg.cols, CV_8UC(1), Scalar::all(0));
		Mat OGGImg(OCImg.rows, OCImg.cols, CV_8UC(1), Scalar::all(0));
		Mat ORGImg(OCImg.rows, OCImg.cols, CV_8UC(1), Scalar::all(0));
		spiltColorToThreeBasicImg(OCImg, OBGImg, OGGImg, ORGImg);
		Mat OBK1Img(OCK1.rows, OCK1.cols, CV_8UC(1), Scalar::all(0));
		Mat OGK1Img(OCK1.rows, OCK1.cols, CV_8UC(1), Scalar::all(0));
		Mat ORK1Img(OCK1.rows, OCK1.cols, CV_8UC(1), Scalar::all(0));
		spiltColorToThreeBasicImg(OCK1, OBK1Img, OGK1Img, ORK1Img);
		Mat OBK2Img(OCK2.rows, OCK2.cols, CV_8UC(1), Scalar::all(0));
		Mat OGK2Img(OCK2.rows, OCK2.cols, CV_8UC(1), Scalar::all(0));
		Mat ORK2Img(OCK2.rows, OCK2.cols, CV_8UC(1), Scalar::all(0));
		spiltColorToThreeBasicImg(OCK2, OBK2Img, OGK2Img, ORK2Img);
		//generate their SK1 and SK2
		Mat BSK1(OBGImg.rows * 2, OBGImg.cols * 2, CV_8UC(1), Scalar::all(0));
		Mat BSK2(OBGImg.rows * 2, OBGImg.cols * 2, CV_8UC(1), Scalar::all(0));
		genGrayStackingKeyPic(path, OBGImg, OBK1Img, OBK2Img, BSK1, BSK2, "BGraySurface");
		Mat GSK1(OGGImg.rows * 2, OGGImg.cols * 2, CV_8UC(1), Scalar::all(0));
		Mat GSK2(OGGImg.rows * 2, OGGImg.cols * 2, CV_8UC(1), Scalar::all(0));
		genGrayStackingKeyPic(path, OGGImg, OGK1Img, OGK2Img, GSK1, GSK2, "GGraySurface");
		Mat RSK1(ORGImg.rows * 2, ORGImg.cols * 2, CV_8UC(1), Scalar::all(0));
		Mat RSK2(ORGImg.rows * 2, ORGImg.cols * 2, CV_8UC(1), Scalar::all(0));
		genGrayStackingKeyPic(path, ORGImg, ORK1Img, ORK2Img, RSK1, RSK2, "RGraySurface");
		//generate color sk1
		mergeThreeBasicImgToColor(SK1, BSK1, GSK1, RSK1);
		mergeThreeBasicImgToColor(SK2, BSK2, GSK2, RSK2);
	}
	void recoverBWStackingPic(Mat& bwO_recover, Mat& SK1, Mat& SK2) {
		subPixelGC SubWhite(6, 2, 2, subPixelGC::White);
		subPixelGC SubBlack(4, 2, 2, subPixelGC::Black);
		subPixelMapping WB2BMap(6, subPixelMapping::Black);
		subPixelMapping WB2WMap(6, subPixelMapping::White);
		bool k1c = false;
		bool k2c = false;
		int k1i = 0;
		int k2i = 0;
		int wI = 0;
		int bI = 0;
		for (int i = 0; i < bwO_recover.rows; i++) {
			for (int j = 0; j < bwO_recover.cols; j++) {
				getRTag(SubBlack, SubWhite, SK1, i, j, k1c, k1i);
				getRTag(SubBlack, SubWhite, SK2, i, j, k2c, k2i);
				if (k1c == k2c) {
					if (k1c == 1) {
						if (k1i == k2i)
							bwO_recover.at<uchar>(i, j) = 255;
						else
							bwO_recover.at<uchar>(i, j) = 0;
					}
					else {
						if ((k1i + k2i) == 7)
							bwO_recover.at<uchar>(i, j) = 0;
						else
							bwO_recover.at<uchar>(i, j) = 255;
					}
				}
				else {
					if (k1c == 0) {
						wI = k1i;
						bI = k2i;
					}
					else {
						wI = k2i;
						bI = k1i;
					}
					if(WB2BMap.data[wI][1] == bI || WB2BMap.data[wI][2] == bI)
						bwO_recover.at<uchar>(i, j) = 0;
					else
						bwO_recover.at<uchar>(i, j) = 255;
				}
			}
		}
	}
	
	string getABSpath(string path) {// return the absPath. supposed the given string is right
									//if path is not abs, replenishing as the current abs path
		size_t found = string(path).find_last_of("/\\");
		if (found == -1) {
			path = string(getcwd(NULL, 0)) + "\\" + path;
		}
		return path;
	}
	string backUpPath(string path) {// return the string of the path of the @path's parent
									// if the @path has no parent return itself
		size_t found = string(path).find_last_of("/\\");
		if (found != -1)
			return path.substr(0, found);
		else
			return path;
	}
	string getPicType(string path) {// get the picture's type
		size_t found = string(path).find_last_of(".");
		if (found != -1)
			return path.substr(found + 1);
		else
			return string("");
	}	
	string genKeyName(string path,string keyName) { // return the path of the pic's key
		return backUpPath(path) + "\\"+keyName+"."+getPicType(path);
	}
	void copySubMatrix(Mat& des, bool** src, int i, int j) {
		//向子秘钥中拷贝子像素块分量
		des.at<uchar>(2 * i, 2 * j) = patch4copySubMatrix(src[1][1]);
		des.at<uchar>(2 * i+1, 2 * j) = patch4copySubMatrix(src[2][1]);
		des.at<uchar>(2 * i, 2 * j+1) = patch4copySubMatrix(src[1][2]);
		des.at<uchar>(2 * i+1, 2 * j+1) = patch4copySubMatrix(src[2][2]);
	}
	uchar patch4copySubMatrix(bool x) {
		if (x)
			return 0;
		else
			return 255;
	}
	bool isDiff(Mat& des, bool** src,int i,int j) {
		for (int oi = 0; oi < 2; oi++)
			for (int oj = 0; oj < 2; oj++) {
				if (des.at<uchar>(2 * i + oi, 2 * j + oj) != patch4copySubMatrix(src[oi + 1][oj + 1]))
					return true;
			}
		return false;			
	}
	int  getSubBlackIndex(subPixelGC& SubBlack, Mat&des, int i, int j) {
		int result = -1;
		for (int index = 1; index <= 4; index++) {
			if (isDiff(des, SubBlack.data[index], i, j))
				continue;
			else {
				result = index;
				break;
			}
		}
		return result;
	}
	int  getSubWhiteIndex(subPixelGC& SubWhite, Mat&des, int i, int j) {
		int result = -1;
		for (int index = 1; index <= 6; index++) {
			if (isDiff(des, SubWhite.data[index], i, j))
				continue;
			else {
				result = index;
				break;
			}
		}
		return result;
	}
	void getRTag(subPixelGC& SubBlack, subPixelGC& SubWhite,Mat&des, int i, int j, bool& color , int& index) {
		//GETRTAG 输入一个2 * 2的数组，判断它是黑子像素块还是白子像素块，并且返回它在相应的子像素块组中的下标
	    // 返回参数 0 是白色 子像素块下标编号1 - 4， 1是黑色 子像素块下标编号1 - 6
		index = getSubBlackIndex(SubBlack, des, i, j);
		if (index == -1) {
			index = getSubWhiteIndex(SubWhite, des, i, j);
			color = 0;
		}
		else {
			color = 1;
		}
			
	}
};

