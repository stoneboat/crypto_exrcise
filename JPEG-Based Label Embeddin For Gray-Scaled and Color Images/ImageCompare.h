#pragma once
#include "stdafx.h"
#include <string>
#include <fstream>
namespace IMagePixelAnaysis {
	const unsigned long BUFLEN = 500;
	class BasicIMagePixelAnaysis {
	public:
		BasicIMagePixelAnaysis(const Mat& a, const Mat& b) {
			imageA.copySize(a);
			imageB.copySize(b);
			a.copyTo(imageA);
			b.copyTo(imageB);
		}
		void generateLog(string fileName="log.txt") {
			assert(imageA.size() == imageB.size());
			ofstream file(fileName);
			char tmpBuf[BUFLEN];
			getTime(tmpBuf);
			file << "the log is generating at " << tmpBuf << endl;
			basicPixelInform(file);
			file.close();
		}
		void basicPixelInform(ofstream & out) {
			unsigned long tmp = 0;
			unsigned int num =0;
			if (imageA.type() <= 3 && imageB.type()<=3) {
				for (int i = 0; i < imageA.rows; i++) {
					uint8_t* f = imageA.ptr<uint8_t>(i);
					uint8_t* d = imageB.ptr<uint8_t>(i);
					for (int j = 0; j < imageA.cols; j++) {
						if (f[j] != d[j]) {
							tmp += abs(f[j] - d[j]);
							num++;
						}
						
					}
				}
				tmp = double(tmp) / (imageA.rows*imageA.cols);
			}
			else {
				for (int i = 0; i < imageA.rows; i++) {
					double* f = imageA.ptr<double>(i);
					double* d = imageB.ptr<double>(i);
					for (int j = 0; j < imageA.cols; j++) {
						if (abs(f[j]- d[j])>0.001) {
							tmp += abs(f[j] - d[j]);
							num++;
						}
					}
				}
				tmp = double(tmp) / (imageA.rows*imageA.cols);
			}
			out << "average pixel differential is	" << tmp << endl;
			out << "different pixel number is	" << num << endl;
		}
	private:
		Mat imageA;
		Mat imageB;
		void getTime(char* tmpBuf) {
			time_t t = time(0);
			strftime(tmpBuf, BUFLEN, "%Y-%m-%d %H:%M:%S", localtime(&t)); //format date     and time.   
		}
	};
};