#include "transportCipher.h"
#include "ctype.h"
TransCypher::TransCypher(int k) {
	key = ((k % 26) + 26) % 26;
	for (int i = 0; i < 26; i++) {
		cipherTable[i] = alphaTable[(i + key) % 26];
	}
}
char TransCypher::Crypt(char x) {
	if (x == ' ')
		return x;
	char temp = toupper(x);
	int index = findAlphaIndex(temp);
	return cipherTable[index];
}
char TransCypher::deCrypt(char x) {
	if (x == ' ')
		return x;
	char temp = toupper(x);
	int index = findCipherIndex(temp);
	return alphaTable[index];
}
char* TransCypher::Crypt(char* str, int l) {
	char* buffer = new char[l+1];
	for (int i = 0; i < l; i++)
		buffer[i] = Crypt(str[i]);
	buffer[l] = '\0';
	return buffer;
}
char* TransCypher::deCrypt(char* str, int l) {
	char* buffer = new char[l+1];
	for (int i = 0; i < l; i++)
		buffer[i] = deCrypt(str[i]);
	buffer[l] = '\0';
	return buffer;
}
int TransCypher::findCipherIndex(char x) {
	for (int i = 0; i < 26; i++)
		if (cipherTable[i] == x)
			return i;
	return -1;
}
int findAlphaIndex(char x) {
	for (int i = 0; i < 26; i++)
		if (alphaTable[i] == x)
			return i;
	return -1;
}