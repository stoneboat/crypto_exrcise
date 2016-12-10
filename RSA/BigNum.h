#pragma once
#include <bitset>
using std::bitset;
class BigNum//�����������������������Ļ� 2060λ��Ϊ1 ����Ϊ0
{
	friend class BigPrime;
private:
	std::bitset<2061> a;    //���Կ��ƴ�����λ�� 
	int len;       //��������
public:
	BigNum() { len = 0; a.reset(); }   //���캯��
	BigNum(const int);       //��һ��int���͵ı���ת��Ϊ����
	BigNum(const char*);     //��һ���ַ������͵ı���ת��Ϊ����
	BigNum(const BigNum &);  //�������캯��
	bool   operator<(const BigNum & T)const;   //��������һ�������Ĵ�С�Ƚ�
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
	BigNum &operator=(const BigNum &);   //���ظ�ֵ�����������֮����и�ֵ����
	BigNum operator+(const BigNum &) const;   //���ؼӷ����������������֮����������
	BigNum operator-(const BigNum &) const;   //���ؼ������������������֮���������㣬ֻʵ�ִ�����С��
	BigNum operator*(const BigNum &) const;   //���س˷����������������֮����������
	BigNum operator/(const BigNum &) const;   //���س������������������֮����������
	BigNum operator%(const BigNum &) const;   //����ȡģ���������������֮���ȡģ���� 
	BigNum operator>>(const int a) const;   //��������λ�����
	BigNum operator << (const int a) const;   //��������λ�����
	void print(char* str, int l);       //�������
	void reset() { (*this).a.reset(); (*this).len = 0; }//������0
	friend BigNum FastPower(const BigNum& a,const BigNum& m,const BigNum& n);
	friend BigNum abs(const BigNum& x) {
		BigNum temp = x;
		temp.a[2047] = 0;
		return temp;
	}
};
//������ a^m % n