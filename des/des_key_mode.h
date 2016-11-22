#pragma once
#include <bitset>
using std::bitset;
/****************************************************************************************************
                                                DES 密钥 类
*****************************************************************************************************/
class KEY_MODE {
public:
	// 密钥构造函数，生成 内部bitset类型 的密钥
	KEY_MODE() {}
	KEY_MODE(unsigned short[8]);
	// 自动生成16轮子密钥存储起来，因为密钥和加密模块彼此分开 
	void Create();
//private:
	//left shit C&D ,producer_Group[turn]
	void LS(const int turn);
	void GetKey(const int turn);
	//test and used once for PC_1 
	bitset<65> primary_key;
	//every sub key producer
	bitset<57> producer_Group[17];
	//every sub key
	bitset<49> key_Group[17]; //key_Group[0] truncate
};