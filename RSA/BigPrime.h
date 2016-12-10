#pragma once
#include "BigNum.h"
class BigPrime {
	friend class RSA;
private:
	BigNum smallPrime[304];//2000以内的小素数共有303 个
	BigNum bigMan;
	BigNum Pre_create(int length);
	bool Rabin_Probability(const BigNum a,const BigNum n,std::ofstream&);
public:
	BigPrime();
	BigNum create(std::ofstream&);
	
};