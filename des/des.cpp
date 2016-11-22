#include "des.h"
#include "des_table.h"
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;
using std::ofstream;
void DES::testCipher() {
	ofstream fout("my.txt",std::ios::app);
	//test cipher key
	/*for (int i = 1; i < 17; i++) {
		for (int j = 1; j < 49; j++)
			fout << key.key_Group[i][j];
		fout << endl;
	}*/
	//test mediate result
	for (int i = 1; i < 65; i++) {
		fout << Producer[i];
	}
	fout << endl;
	fout.close();
}
DES::DES(unsigned short message[8]) {
	bitset<8> temp;
	bitset<9> reg;
	for (int i = 0; i < 8; i++) {
		temp = bitset<8>(message[i]);
		for (int j = 1; j < 9; j++) {
			reg[j] = temp[8 - j];
			Message[i * 8 + j] = reg[j];
		}
	}
	/*bitset<8> temp;
	for (int i = 0; i < 8; i++) {
		temp = bitset<8>(message[i]);
		for (int j = 1; j < 9; j++) {
			Message[i * 8 + j] = temp[j-1];
		}
	}*/
}
void DES::PermutationIP() {
	for (int i = 1; i < 65; i++)
		Producer[i] = Message[IP[i]];
}
void DES::PermutationVIP() {
	for (int i = 1; i < 65; i++)
		CipherText[i] = Producer[IP_V[i]];
}
void DES::GetCiypher(unsigned short*  result) {
	for (int i = 0; i < 8; i++) {
		unsigned int temp1 = 0;
		unsigned int temp2 = 0;
		unsigned int temp = 0;
		for (int j = 1; j < 5; j++) {
			temp1 *= 2;
			temp1 += CipherText[i * 8 + j];
		}
		for (int j = 5; j < 9; j++) {
			temp2 *= 2;
			temp2 += CipherText[i * 8 + j];
		}
		temp = temp1 * 16 + temp2;
		result[i] = temp;
	}
}
void DES::SubstituionS() {
	bitset<2> selectLine;
	bitset<4> selectRow;
	bitset<4> num ;
	for (int Line = 1; Line < 9; Line++) {
		//analysis S line
		selectLine[0] = temp[(Line - 1) * 6 + 6];
		selectLine[1] = temp[(Line - 1) * 6 + 1];
		//analysis S Row
		selectRow[0] = temp[(Line - 1) * 6 + 5];
		selectRow[1] = temp[(Line - 1) * 6 + 4];
		selectRow[2] = temp[(Line - 1) * 6 + 3];
		selectRow[3] = temp[(Line - 1) * 6 + 2];
		//get Num
		num = S[Line][selectLine.to_ulong()][selectRow.to_ulong()];
		//get Result
		tempS[(Line - 1) * 4 + 1] = num[3];
		tempS[(Line - 1) * 4 + 2] = num[2];
		tempS[(Line - 1) * 4 + 3] = num[1];
		tempS[(Line - 1) * 4 + 4] = num[0];
	}
}
void DES::Turn(int turn) {
	//getPrepare
	for (int i = 1; i < 33; i++)
		TempL[i] = Producer[i];
	for (int i = 33; i < 65; i++)
		TempR[i-32] = Producer[i];
	//swap get Producer Left 32 bits
	for (int i = 1; i < 33; i++)
		Producer[i] = TempR[i];
	//Extending Permutation
	for (int i = 1; i < 49; i++)
		temp[i] = TempR[EP[i]];
	//XOR with Key[turn]
	for (int i = 1; i < 49; i++)
		temp[i] = temp[i] ^ key.key_Group[turn][i];
	//S box substitute
	SubstituionS();
	//Permutation P
	for (int i = 1; i < 33; i++)
		TempR[i] = tempS[P[i]];
	//XOR with L former
	for (int i = 1; i < 33; i++)
		TempL[i] = TempR[i] ^ TempL[i];
	//swap get Producer Right 32 bits 
	for (int i = 33; i < 65; i++)
		Producer[i] = TempL[i - 32];
	//testCipher();
}
void DES::CipherCreate(unsigned short key[8]) {
	//prepare subkey
	this->key = KEY_MODE(key);
	this->key.Create();
	PermutationIP();
	for (int i = 1; i < 17; i++) 
		Turn(i);
	// L R swap
	for (int i = 1; i < 33; i++)
		TempL[i] = Producer[i];
	for (int i = 33; i < 65; i++)
		TempR[i - 32] = Producer[i];
	//swap get Producer Left 32 bits
	for (int i = 1; i < 33; i++)
		Producer[i] = TempR[i];
	//swap get Producer Right 32 bits 
	for (int i = 33; i < 65; i++)
		Producer[i] = TempL[i - 32];
	PermutationVIP();
}
void DES::CipherDeCreate(unsigned short key[8]) {
	//prepare subkey
	this->key = KEY_MODE(key);
	this->key.Create();
	PermutationIP();
	for (int i = 1; i < 17; i++)
		Turn(17-i);
	// L R swap
	for (int i = 1; i < 33; i++)
		TempL[i] = Producer[i];
	for (int i = 33; i < 65; i++)
		TempR[i - 32] = Producer[i];
	//swap get Producer Left 32 bits
	for (int i = 1; i < 33; i++)
		Producer[i] = TempR[i];
	//swap get Producer Right 32 bits 
	for (int i = 33; i < 65; i++)
		Producer[i] = TempL[i - 32];
	PermutationVIP();
}
/*int main() {
unsigned short message[8] = { 0x82, 0xDC, 0xBA, 0xFB, 0xDE, 0xAB, 0x66, 0x02 };
unsigned short key[8] = { 0x10, 0x31, 0x6E, 0x02, 0x8C, 0x8F, 0x3B, 0x4A };
unsigned short k[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
DES test(message);
unsigned short* result = new unsigned short[8];
test.CipherDeCreate(key);
test.GetCiypher(result);
//for (int i = 0; i < 9; i++)
//std::cout << test.primary_key[3][i] << "  ";
//test.Create();
int i;
for (int i = 0; i < 8; i++)
	cout << result[i] << endl;
delete result;
return 0;
}*/