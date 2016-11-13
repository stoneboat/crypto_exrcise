#include "permutationCipher.h"
#include<fstream>
#include <iostream>
#include <queue>
#include <iomanip>
using namespace std;
const int massageNum = 406;
const int maxwordLen = 50;
int spaceNum = 0;
int alphaNum = 406;
double alphaOdds[26];
const char DecryptMessage[407] = "SIC GCBSPNA XPMHACQ JB GPYXSMEPNXIY JR SINS MF SPNBRQJSSJBE JBFMPQNSJMB FPMQ N XMJBS N SM N XMJBS H HY QCNBR MF N XMRRJHAY JBRCGZPC GINBBCA JB RZGI N VNY SINS SIC MPJEJBNA QCRRNEC GNB MBAY HC PCGMTCPCD HY SIC PJEISFZA PCGJXJCBSR SIC XNPSJGJXNBSR JB SIC SPNBRNGSJMB NPC NAJGC SIC MPJEJBNSMP MF SIC QCRRNEC HMH SIC PCGCJTCP NBD MRGNP N XMRRJHAC MXXMBCBS VIM VJRICR SM ENJB ZBNZSIMPJOCD GMBSPMA MF SIC QCRRNEC";
struct word {
	char name[maxwordLen];
	int time;
	double odds;
	word() {
		time = 0;
		for (int i = 0; i < maxwordLen; i++)
			name[i] = '\0';
	}
	word(char* str) {
		time = 1;
		strcpy_s(name, str);
	}
	bool operator < (const word &x) const
	{
		return time<x.time;
	}
};
struct alpha {
	char name;
	int time;
	double odds;
	alpha(char c,int t,double o) {
		name = c;
		time = t;
		odds = o;
	}
	alpha() {
		name = 0;
		time = 0;
		odds = 0;
	}
	bool operator < (const alpha &x) const
	{
		return time<x.time;
	}
};
int findWord(char*, word* wordlist, int len);//char* var must terminated by '\0'
int wordnum = 0;
int main() {
	priority_queue<alpha> alp;
	priority_queue<word> wo;
	ofstream fout("report.txt");
	//统计字母频率
	char alphaTime[26];
	for (int i = 0; i < 26; i++)
		alphaTime[i] = 0;
	int index;
	for (int i = 0; i < massageNum; i++) {
		if (DecryptMessage[i] == ' ') {
			spaceNum++;
			continue;
		}
		index = findAlphaIndex(DecryptMessage[i]);
		alphaTime[index]++;
	}
	alphaNum = massageNum - spaceNum;
	for (int i = 0; i < 26; i++) {
		alphaOdds[i] = double(alphaTime[i]) / alphaNum;
		alphaOdds[i] = alphaOdds[i] * 100;
		if (!alphaTime[i])
			continue;
		alp.push(alpha(alphaTable[i], alphaTime[i], alphaOdds[i]));
	}
	fout << setw(15) <<"字母" << ":   " << "字母统计数（百分比）" << endl;
	alpha temp;
	while (!alp.empty()) {
		temp = alp.top();
		fout << setw(15) << temp.name << ":   " << temp.odds << endl;
		alp.pop();
	}
	fout << endl;
	fout << endl;
	ifstream fin("decryptMessage.txt");
	char buffer[maxwordLen];
	word wordlist[50];
	index = -1;
	while (!fin.eof()) {
		fin >> buffer;
		index = findWord(buffer, wordlist, wordnum);
		if (index == -1)
			wordlist[wordnum++] = word(buffer);
		else
			wordlist[index].time++;
	}
	for (int i = 0; i < wordnum; i++) {
		wordlist[i].odds = double(wordlist[i].time) / wordnum * 100;
		wo.push(wordlist[i]);
	}
		
	fout << "样本事例单词总数：\t" << wordnum << endl;
	fout << setw(15) << "单词" << ":   " << "单词统计数（百分比）" << endl;
	word tempW;
	while (!wo.empty()) {
		tempW = wo.top();
		fout << setw(15) << tempW.name << ":   " << tempW.odds << endl;
		wo.pop();
	}
	int a = 0;
	fin.close();
	fout.close();
	return 0;
}
int findWord(char* name,word* wordlist, int len) {
	for (int i = 0; i < len; i++)
		if (strcmp(wordlist[i].name, name)==0)
			return i;
	return -1;
}