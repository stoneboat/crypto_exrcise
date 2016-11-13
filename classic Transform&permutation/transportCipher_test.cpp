/*transportCipher.h  test cpp
*/
#include"transportCipher.h"
# include<iostream>
using namespace std;
const int max = 10000;
int main() {
	char str[max];
	int key;
	char* decryptMessage;
	char* Message;
	cout << "type in key:";
	cin >> key;
	cin.get();
	TransCypher x = TransCypher(key);
	cout << "type in one line test message\n";
	cin.getline(str, max);
	int len = strlen(str);
	decryptMessage =x.Crypt(str, len);
	cout << "crypt message:   "<<decryptMessage << endl;
	Message = x.deCrypt(decryptMessage, len);
	cout << "decrypt message: " << Message << endl;
	delete[] decryptMessage;
	delete[] Message;

	/* try to attack a transportCipher
	*/
	/*char test_str[max];
	int test_key=0;
	
	cout << "type in one line test decrypt message\n";
	cin.getline(test_str, max);
	int test_len = strlen(test_str);
	for (int i = 0; i < 26; i++) {
		test_key = i;
		TransCypher x = TransCypher(test_key);
		char* test_message = x.deCrypt(test_str, test_len);
		cout << "testkey: " << test_key << "  decrypt message: " << test_message << endl;
		delete[] test_message;
	}*/
}