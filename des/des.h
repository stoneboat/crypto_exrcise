#pragma once
#include <bitset>
#include "des_key_mode.h"
using std::bitset;
/****************************************************************************************************
                                              DES º”√‹ ¿‡
*****************************************************************************************************/
class DES {
public:
	DES(unsigned short Message[8]);
	void CipherCreate(unsigned short key[8]);
	void CipherDeCreate(unsigned short key[8]);
	void Turn(int turn);
	//must be an 8 long unsigned short array
	void GetCiypher(unsigned short*);
	void testCipher();
	void PermutationIP();
	void PermutationVIP();
	void SubstituionS();
	KEY_MODE key;
	bitset<65> Message;
	bitset<33> TempL;
	bitset<33> TempR;
	bitset<49> temp;
	bitset<33> tempS;
	bitset<65> Producer;
	bitset<65> CipherText;
};