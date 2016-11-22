#include "des_key_mode.h"
#include "des_table.h"
#include <iostream>
KEY_MODE::KEY_MODE(unsigned short message[8]) {
	bitset<8> temp;
	bitset<9> reg;
	for (int i = 0; i < 8; i++) {
		temp = bitset<8>(message[i]);
		for (int j = 1; j < 9; j++) {
			reg[j] = temp[8-j];
			primary_key[i * 8 + j] = reg[j];
		}
	}
}
void KEY_MODE::LS(const int turn) {
	int post = 0;
	//Cturn LS
	for (int t = 1; t < 29; t++) {
		post = (t + 28 - LST[turn]) % 28;
		if (post == 0) 
			post += 28;
		producer_Group[turn][post] = producer_Group[turn - 1][t];
	}
	//Dturn LS
	for (int t = 1; t < 29; t++) {
		post = (t + 28 - LST[turn]) % 28;
		if (post == 0) 
			post += 28;
		producer_Group[turn][post+28] = producer_Group[turn - 1][t+28];
	}
}
void KEY_MODE::GetKey(const int turn) {
	for (int i = 1; i < 49; i++)
		key_Group[turn][i] = producer_Group[turn][int(PC_2[i])];
}
void KEY_MODE::Create() {
	//compressing PC_1
	UINT post = 0;
	for (int i = 1; i < 57; i++) {
		producer_Group[0][i] = primary_key[int(PC_1[i])];
	}
	// get 16 turns compressing PC_2 and key
	for (int i = 1; i < 17; i++) {
		LS(i);
		GetKey(i);
	}
}
// test func
/*int main() {
	unsigned short key[8] = { 0x10, 0x31, 0x6E, 0x02, 0x8C, 0x8F, 0x3B, 0x4A };
	KEY_MODE test(key);
	for (int i = 0; i < 9; i++)
		std::cout << test.primary_key[3][i] << "  ";
	test.Create();
	return 0;
}*/