#pragma once
#include <bitset>
using std::bitset;
class BigNum//大整数加入了正负，负数的话 2060位置为1 否则为0
{
	friend class BigPrime;
private:
	std::bitset<2061> a;    //可以控制大数的位数 
	int len;       //大数长度
public:
	BigNum() { len = 0; a.reset(); }   //构造函数
	BigNum(const int);       //将一个int类型的变量转化为大数
	BigNum(const char*);     //将一个字符串类型的变量转化为大数
	BigNum(const BigNum &);  //拷贝构造函数
	bool   operator<(const BigNum & T)const;   //大数和另一个大数的大小比较
	bool   operator>(const BigNum & T)const;
	bool   operator>=(const BigNum & T)const;
	bool   operator<=(const BigNum & T)const;
	bool   operator==(const BigNum & T)const;
	bool   operator!=(const BigNum & T)const{
		if ((*this).len != T.len)
			return true;
		if ((*this) == T)
			return false;
		else
			return true;
	}
	BigNum &operator=(const BigNum &);   //重载赋值运算符，大数之间进行赋值运算
	BigNum operator+(const BigNum &) const;   //重载加法运算符，两个大数之间的相加运算
	BigNum operator-(const BigNum &) const;   //重载减法运算符，两个大数之间的相减运算，只实现大数减小数
	BigNum operator*(const BigNum &) const;   //重载乘法运算符，两个大数之间的相乘运算
	BigNum operator/(const BigNum &) const;   //重载除法运算符，两个大数之间的相除运算
	BigNum operator%(const BigNum &) const;   //重载取模运算符，两个大数之间的取模运算 
	BigNum operator>>(const int a) const;   //重载右移位运算符
	BigNum operator << (const int a) const;   //重载左移位运算符
	void print(char* str, int l);       //输出大数
	void reset() { (*this).a.reset(); (*this).len = 0; }//大数置0
	friend BigNum FastPower(const BigNum& a,const BigNum& m,const BigNum& n);
	friend BigNum abs(const BigNum& x) {
		BigNum temp = x;
		temp.a[2047] = 0;
		return temp;
	}
};
//快速幂 a^m % n