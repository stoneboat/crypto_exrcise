#include "BigPrime.h"
#include <bitset>
#include <iostream>
#include <random>
#include <time.h>
#include <fstream>
using std::cout;
using std::endl;
BigPrime::BigPrime() {
	bitset<2000> tempFilter;
	tempFilter.set();
	tempFilter[1] = 0;
	tempFilter[0] = 0;
	int filter = 2;
	while (filter<50)
	{
		for (int i = filter + 1; i < 2000; i++)
			if (i%filter == 0)
				tempFilter[i] = 0;
		for (int i = filter + 1; i < 2000; i++) {
			if (tempFilter[i]) {
				filter = i;
				break;
			}
		}
	}
	int count = 0;
	for (int i = 2; i < 2000; i++)
		if (tempFilter[i])
			smallPrime[++count] = BigNum(i);
	bigMan.reset();
}
bool BigPrime::Rabin_Probability(const BigNum a,const BigNum n,std::ofstream& out) {
	BigNum d(1);
	BigNum one(1);
	BigNum x;
	BigNum m = n - d;
	for (int k = m.len - 1; k >= 0; k--) {
		x = d;
		d = d*d;
		d = d%n;
		if (d == one&&x != one&&x != m)
			return false;
		if (m.a[k]) {
			d = d*a;
			d = d%n;
		}
		cout << k << " ";
		out << k << " ";
	}
	cout << endl;
	out << endl;
	if (d != one)
		return false;
	else

		return true;
	
}
BigNum BigPrime::Pre_create(int length) {
	srand((unsigned int)(time(NULL)));
	bigMan.reset();
	bigMan.len = length;
	bigMan.a[0] = 1;
	bigMan.a[length-1] = 1;
	for (int i = 1; i < length-1; i++)
		bigMan.a[i] = bool(rand() % 2);
	return bigMan;
}
BigNum BigPrime::create(std::ofstream& out) {
	BigNum zero;
	BigNum two(2);
	BigNum temp = Pre_create(512);

	//log
	out << "Pre create success\n";
	cout << "Pre create success\n";
	//

	bool flag = true;
	while (true)
	{
		int i = 1;
		for (i = 1; i < 304; i++) {
			out << "pass small Prime  " << i << "\n";
			cout << "pass small Prime  " << i << "\n";
			if (temp%smallPrime[i] == zero) {
				temp = temp + two;
				out << " number failed ,Pre create success\n";
				cout << " number failed ,Pre create success\n";
				break;
			}
		}
		if (i == 304) {
			out << "Pass Small Prime test Successfully\n";
			cout << "Pass Small Prime test Successfully\n";
			if (this->Rabin_Probability(BigNum(2), temp,out)) {
				out << " Rabin test 1 turn success\n";
				cout << " Rabin test 1 turn success\n";
			}
			else {
				out<< " Rabin test 1 turn failed\n";
				cout << " Rabin test 1 turn failed\n";
				temp = temp + two;
				continue;
			}
			if (this->Rabin_Probability(BigNum(3), temp,out)) {
				out << " Rabin test 2 turn success\n";
				cout << " Rabin test 2 turn success\n";
			}
			else {
				out << " Rabin test 2 turn failed\n";
				cout << " Rabin test 2 turn failed\n";
				temp = temp + two;
				continue;
			}
			if (this->Rabin_Probability(BigNum(4), temp,out)) {
				out << " Rabin test 3 turn success\n";
				cout << " Rabin test 3 turn success\n";
			}
			else {
				out << " Rabin test 3 turn failed\n";
				cout << " Rabin test 3 turn failed\n";
				temp = temp + two;
				continue;
			}
			if (this->Rabin_Probability(BigNum(5), temp,out)) {
				out << " Rabin test 4 turn success\n";
				cout << " Rabin test 4 turn success\n";
			}
			else {
				out << " Rabin test 4 turn failed\n";
				cout << " Rabin test 4 turn failed\n";
				temp = temp + two;
				continue;
			}
			if (this->Rabin_Probability(BigNum(6), temp,out)) {
				out << " Rabin test 5 turn success\n";
				cout << " Rabin test 5 turn success\n";
			}
			else {
				out << " Rabin test 5 turn failed\n";
				cout << " Rabin test 5 turn failed\n";
				temp = temp + two;
				continue;
			}
			out << "Prime make success\n";
			char test[129];
			test[128] = '\0';
			temp.print(test, 128);
			out << test << endl;
			break;
		}
			
		out << endl;
	}			
	return temp;
}
/*int main() {
	BigPrime a;
	BigNum c(2);
	//int x = 10001521;
	BigNum b("9B3FA11F78307653");
	//BigNum zero;
	std::ofstream fout("Prime.txt",std::ios::app);
	
	bool x = a.Rabin_Probability(c, b,fout);// a.create();
	//BigNum d = e*c;
	if(x)
		cout << "x: " << x << endl;
	//c= c << 2;
	//BigNum d = a.create(fout);
	fout.close();
	char test[129];
	//test[128] = '\0';
	//d.print(test, 128);
	//cout << test << endl;
	return 0;
}*/