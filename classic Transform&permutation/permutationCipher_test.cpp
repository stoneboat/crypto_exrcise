#include <iostream>
# include "permutationCipher.h"
using namespace std;
const int max = 10000;
int main() {
	char str[max];
	char key[max];
	int key_len = 0;
	char* decryptMessage;
	char* Message;
	cout << "type in key:";
	cin.getline(key, max);
	key_len = strlen(key);
	PermutaCypher x = PermutaCypher(key);
	cout << "type in one line test message\n";
	cin.getline(str, max);
	int len = strlen(str);
	decryptMessage =x.Crypt(str, len);
	cout << "crypt message:   "<<decryptMessage << endl;
	Message = x.deCrypt(decryptMessage, len);
	cout << "decrypt message: " << Message << endl;
	delete[] decryptMessage;
	delete[] Message;
	return 0;
}