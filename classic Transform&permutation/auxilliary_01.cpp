#include<fstream>
#include<iostream>
#include <regex>
using namespace std;
const int maxwordLen = 8;
int main() {
	//([a-z])\1{2}
	//std::string pattern("[a-z]e([a-z]\\1{1})a[a-z]e");
	//std::string pattern("[a-z]oust");
	//std::string pattern("[a-z]aus");
	//std::string pattern("a[a-z]e");
	//std::regex r("([a-z])\\1{1}");
	//std::regex r("[a-z]e([a-z])\\1{1}a[a-z]e");
	std::regex r("[A-Z]E([A-Z])\\1{1}A[A-Z]E");
	char buffer[maxwordLen];
	ifstream fin("7letterwords.txt");
	ofstream fout("auxiliary01Resut.txt");
	int count = 0;
	while (!fin.eof()) {
	   fin >> buffer;
	   if(std::regex_match(buffer, r))
		   fout<<buffer<<'\n';
	   //count++;
	   //cout << count << endl;
	}
	fin.close();
	fout.close();
	//std::regex r("[a-z]e([a-z])\\1{1}a[a-z]e");
	//cout << std::regex_match("message", r);
	return 0;
}