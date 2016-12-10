#pragma once
#include "BigNum.h"
#include "BigPrime.h"
BigNum zeroEuclid;
BigNum oneEuclid(1);
struct  EXT_Group
{
	EXT_Group(const BigNum& a, const BigNum& b, const BigNum& c) {
		one = a;
		two = b;
		thr = c;
	}
	EXT_Group(const EXT_Group &T) {
		one = T.one;
		two = T.two;
		thr = T.thr;
	}
	EXT_Group &operator=(const EXT_Group & T) {
		return EXT_Group(T.one, T.two, T.thr);
	}
	BigNum one;
	BigNum two;
	BigNum thr;
};

BigNum Euclid(const BigNum& f, const BigNum& d) {
	BigNum x;
	BigNum y;
	if (f > d) {
		x = f;
		y = d;
	}
	else {
		x = d;
		y = f;
	}
	if (y == zeroEuclid)
		return x;
	if (y == oneEuclid)
		return y;
	BigNum r = x%y;
	return(Euclid(y, r));
}
BigNum ExtEuclid(const EXT_Group& Xgroup, const EXT_Group& Ygroup) {
	BigNum Q;
	if (Ygroup.thr == zeroEuclid)
		return zeroEuclid;//no inversed number
	if (Ygroup.thr == oneEuclid)
		return Ygroup.two;
	Q = Xgroup.thr / Ygroup.thr;
	EXT_Group Tgroup(Xgroup.one - Q*Ygroup.one, Xgroup.two - Q*Ygroup.two, Xgroup.thr - Q*Ygroup.thr);
	return ExtEuclid(Ygroup, Tgroup);
}
BigNum GetInversed(const BigNum& f, const BigNum& d) {//f是模数，d是求逆的元
	BigNum ff;
	BigNum dd;
	if (f > d) {
		ff = f;
		dd = d;
	}
	else {
		ff = f;
		dd = d % f;
	}
	EXT_Group Xgroup(oneEuclid, zeroEuclid, ff);
	EXT_Group Ygroup(zeroEuclid, oneEuclid, dd);
	BigNum result= ExtEuclid(Xgroup, Ygroup);
	if (result < zeroEuclid)
		result = result + f;
	return result;
}
class RSA {
public:
	struct
	{
		BigNum n;
		BigNum e;
	}Pub_Key;
	struct
	{
		BigNum n;
		BigNum d;
	}Pri_Key;
	RSA(const BigNum &p, const BigNum &q) {
		BigNum one(1);
		BigNum n = p*q;
		BigNum phi = (p - one)*(q - one);
		Pub_Key.n = n;
		for (int i = 2;; i++) { //小数放水ing。。
			if (phi%BigNum(i) == 0)
				continue;
			else {
				Pub_Key.e = i;
				break;
			}
		}
		Pri_Key.n = n;
		Pri_Key.d = GetInversed(phi, Pub_Key.e);

	}
	BigNum Cipher(BigNum m);
	BigNum DeCipher(const BigNum& );
};