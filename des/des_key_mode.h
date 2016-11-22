#pragma once
#include <bitset>
using std::bitset;
/****************************************************************************************************
                                                DES ��Կ ��
*****************************************************************************************************/
class KEY_MODE {
public:
	// ��Կ���캯�������� �ڲ�bitset���� ����Կ
	KEY_MODE() {}
	KEY_MODE(unsigned short[8]);
	// �Զ�����16������Կ�洢��������Ϊ��Կ�ͼ���ģ��˴˷ֿ� 
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