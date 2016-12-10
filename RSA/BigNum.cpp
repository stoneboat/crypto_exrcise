#include "BigNum.h"
#include <iostream>
using std::cout;
using std::endl;
BigNum::BigNum(const int b)     //将一个int类型的变量转化为大数
{
	int temp = abs(b);
	len = 0;
	while (temp) {
		a[len++] = temp % 2;
		temp /= 2;
	}
	if (b < 0)
		a[2060] = 1;
}
BigNum::BigNum(const char*s)     //将一个字符串类型的变量转化为大数,只能是正数转换，暂不接受负数
{
	len = 0;
	int l = strlen(s);
	int temp = 0;
	for (int i = 0; i < l; i++) {
		if (s[l-i-1] >= 65)
			temp = s[l-i-1] - 55;
		else 
			temp = s[l-i-1]-48;
		for (int j = 0; j<4; j++) {
			a[len++] = temp % 2;
			temp /= 2;
		}
	}
	while (!a[len])
		len--;
	len++;
}
void BigNum::print(char* str, int l)    //输出大数
{
	int sl;
	bitset<4> temp;
	if (len % 4)
		sl = len / 4 + 1;
	else
		sl = len / 4;
	if (sl > l)
		throw;
	if (this->a[2060])
		str[0] = '-';
	for (int i = 0; i < sl; i++) {
		temp[0] = a[i * 4];
		temp[1] = a[i * 4 + 1];
		temp[2] = a[i * 4 + 2];
		temp[3] = a[i * 4 + 3];
		if(temp.to_ulong()<10)
			str[sl - i-1] = temp.to_ulong() + 48;
		else
			str[sl - i-1] = temp.to_ulong() + 55;
	}
}
BigNum::BigNum(const BigNum & T) : len(T.len)  //拷贝构造函数
{
	for(int i = 0 ; i < len ; i++)
		a[i] = T.a[i];
	a[2060] = T.a[2060];
}
BigNum & BigNum::operator=(const BigNum & n)   //重载赋值运算符，大数之间进行赋值运算
{
	this->a.reset();
	int i;
	len = n.len;
	for (i = 0; i < len; i++)
		a[i] = n.a[i];
	a[2060] = n.a[2060];
	return *this;
}
BigNum BigNum::operator+(const BigNum & T) const   //两个大数之间的相加运算 用真值表来算
{
	if (((!this->a[2060]) && (!T.a[2060])) || ((this->a[2060]) && (T.a[2060]))) {
		//下面是两个正或者负的大整数相加的情况
		BigNum t(*this);
		int big;      //位数   
		bool carry = 0;
		big = T.len > len ? T.len : len;
		for (int i = 0; i < big; i++) {
			t.a[i] = (~this->a[i] & T.a[i] & ~carry) | (this->a[i] & ~T.a[i] & ~carry) | (~this->a[i] & ~T.a[i] & carry) | (this->a[i] & T.a[i] & carry);
			carry = (carry&T.a[i]) | (this->a[i] & T.a[i] & ~carry) | (this->a[i] & ~T.a[i] & carry);
		}
		if (carry) {
			t.a[big] = 1;
			t.len = big + 1;
		}
		else
			t.len = big;
		t.a[t.len] = len;
		return t;
	}
	else {//转化成正数减正数的情况
		if (this->a[2060]) {
			BigNum temp(*this);
			temp.a[2060] = 0;
			return T - temp;
		}
		else {
			BigNum temp(T);
			temp.a[2060] = 0;
			return (*this)-temp;
		}
	}
	
	
} 
BigNum BigNum::operator >> (const int shift) const {
	BigNum t(*this);
	if (shift >= t.len) {
		t.a.reset();
		t.len = 0;
	}
		
	else {
		t.len -= shift;
		for (int i = 0; i < len; i++)
			t.a[i] = t.a[i + shift];
		for (int i = t.len; i < t.len + shift; i++)
			t.a[i] = 0;
	}
	return t;
}
BigNum BigNum::operator << (const int shift) const {
	BigNum t(*this);
	if (t.len == 0)
		return t;
	if (shift+t.len >= 2060) {
		throw;
	}
	else {
		for (int i = this->len-1; i >= 0; i--)
			t.a[i+shift] = this->a[i];
		t.len += shift;
		for (int i = 0; i < shift; i++)
			t.a[i] = 0;
	}
	return t;
}
BigNum BigNum::operator*(const BigNum & T) const {//T 是乘数 this是被乘数 t 保存结果
	BigNum t(*this);
	BigNum result;
	t.a[2060] = 0;
	for (int i = T.len-1; i >=0 ; i--) {
		if (T.a[i]) {
			result = result + t;
			result = result << 1;
		}
		else
			result = result << 1;
	}
	result = result >> 1;
	if ((this->a[2060] && T.a[2060]) || ((!this->a[2060]) && (!T.a[2060])))
		result.a[2060] = 0;
	else
		result.a[2060] = 1;
	return result;
}
bool   BigNum::operator<(const BigNum & T)const {
	if ((!this->a[2060]) && (!T.a[2060])) {
		if (this->len < T.len)
			return true;
		if (this->len > T.len)
			return false;
		for (int i = T.len - 1; i >= 0; i--) {
			if (this->a[i] > T.a[i])
				return false;
			else if (this->a[i] < T.a[i])
				return true;
			else continue;
		}
		return false;
	}
	else if ((this->a[2060]) && (T.a[2060])) {
		if (this->len < T.len)
			return false;
		if (this->len > T.len)
			return true;
		for (int i = T.len - 1; i >= 0; i--) {
			if (this->a[i] > T.a[i])
				return true;
			else if (this->a[i] < T.a[i])
				return false;
			else continue;
		}
		return false;
	}
	else if (this->a[2060]) {
		return true;
	}
	else {
		return false;
	}
	
}
bool   BigNum::operator>(const BigNum & T)const {
	if ((*this) <= T)
		return false;
	else
		return true;
}
bool   BigNum::operator==(const BigNum & T)const {
	if (this->len != T.len)
		return false;
	for (int i = 0; i < len; i++) {
		if (this->a[i] == T.a[i])
			continue;
		else
			return false;
	}
	if (a[2060] != T.a[2060])
		return false;
	return true;
}
bool   BigNum::operator<=(const BigNum & T)const {
	if (*this < T || *this == T)
		return true;
	else
		return false;
}
bool   BigNum::operator>=(const BigNum & T)const {
	if ((*this) < T)
		return false;
	else 
		return true;
}
BigNum BigNum::operator-(const BigNum & T) const {
	BigNum big;
	BigNum small;
	BigNum temp;
	bool flag;
	if (((!this->a[2060]) && (!T.a[2060]))) {
		if ((*this) > T) {
			big = *this;
			small = T;
			flag = 0;
		}
		else {
			big = T;
			small =  *this;
			flag = 1;
		}
	}
	else if ((this->a[2060]) && (T.a[2060])) {
		if ((*this) > T) {
			big = T;
			big.a[2060] = 0;
			small = *this;
			small.a[2060] = 0;
			flag = 1;
		}
		else {
			big = *this;
			big.a[2060] = 0;
			small = T;
			small.a[2060] = 0;
			flag = 0;
		}
	}
	else if (!this->a[2060]) {
		temp = T;
		temp.a[2060] = 0;
		return (*this) + temp;
	}
	else {
		temp = T;
		temp.a[2060] = 1;
		return (*this) + temp;
	}
		BigNum t;
		t.len = big.len;
		if (*this == T)
			return BigNum();
		bool carry = 0;
		for (int i = 0; i < t.len; i++) {
			if (big.a[i] > small.a[i] && carry) {
				carry = 0;
				t.a[i] = 0;
			}
			else if (big.a[i] > small.a[i]) {
				t.a[i] = 1;
			}
			else if (big.a[i] == small.a[i] && carry) {
				t.a[i] = 1;
			}
			else if (big.a[i] == small.a[i]) {
				t.a[i] = 0;
			}
			else if (big.a[i] < small.a[i] && carry) {
				t.a[i] = 0;
				carry = true;
			}
			else {
				t.a[i] = 1;
				carry = true;
			}
		}
		while (!t.a[t.len])
		{
			t.len--;
		}
		t.len++;
		t.a[2060] = flag;
		return t;
	
}
BigNum BigNum::operator%(const BigNum & T) const {
	if (T.a[2060])//模数应该为正
		throw;
	int flag = 0;
	if (this->a[2060]) 
		flag = 1;
	BigNum t = abs(*this);
	BigNum temp = T;
	while (t > temp) 
		temp = temp << 1;
	while (temp >= T) {
		if (temp <= t)
			t = t - temp;
		temp = temp >> 1;
	}
	if (flag)
		t = T - t;
	return t;
}
BigNum BigNum::operator/(const BigNum & T) const {
	bool flag = 0;
	if ((this->a[2060] && T.a[2060]) || ((!this->a[2060]) && (!T.a[2060]))) 
		flag = 0;
	else
		flag = 1;
	if ((*this) < T)
		return BigNum();
	BigNum result;
	BigNum count(1);
	BigNum t= abs(*this);
	BigNum temp = abs(T);
	BigNum sta = abs(T);
	while (t > temp) {
		temp = temp << 1;
		count = count << 1;
	}
	while (temp >= sta) {
		if (temp <= t) {
			t = t - temp;
			result = result + count;
		}
		count = count >> 1;
		temp = temp >> 1;
	}
	return result;
}
BigNum FastPower(const BigNum& a,const BigNum& m,const BigNum& n) {
	BigNum d(1);
	for (int i = m.len - 1; i >= 0; i--) {
		d = d*d;
		d = d%n;
		if (m.a[i]) {
			d = d*a;
			d = d%n;
		}
		cout << i << " ";
	}
	cout << endl;
	return d;
}

/*int main(void)
{
	BigNum a(-9);
	BigNum b(2);
	BigNum c(3);
	BigNum d = a*b;
	//c= c << 2;
	char test[10];
	test[9] = '\0';
	d.print(test, 9);
	cout << test << endl;

}*/