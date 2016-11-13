#include"permutationCipher.h"
#include<iostream>
#include <ctype.h>
PermutaCypher::PermutaCypher(char* key) {
	strcpy_s(this->key, key);
	int len = strlen(key);
	this->key[len] = '\0';
	for (int i = 0; i < 26; i++) cipherTable[i] = '0';
	int now = 0;
	for (int i = 0; i < len; i++) {
		if (key[i] == ' ' || findDuplicate(toupper(key[i]), now))
			continue;
		if (!isalpha(key[i]))
			throw;
		cipherTable[now++] = toupper(key[i]);
	}
	for (int i = 0; i < 26; i++)
		if (findDuplicate(alphaTable[i], now))
			continue;
		else
			cipherTable[now++] = alphaTable[i];
}
char PermutaCypher::Crypt(char x) {
	if (x == ' ')
		return x;
	char temp = toupper(x);
	int index = findAlphaIndex(temp);
	return cipherTable[index];
}
char PermutaCypher::deCrypt(char x) {
	if (x == ' ')
		return x;
	char temp = toupper(x);
	int index = findCipherIndex(temp);
	return alphaTable[index];
}
char* PermutaCypher::Crypt(char* str, int l) {
	char* buffer = new char[l + 1];
	for (int i = 0; i < l; i++)
		buffer[i] = Crypt(str[i]);
	buffer[l] = '\0';
	return buffer;
}
char* PermutaCypher::deCrypt(char* str, int l) {
	char* buffer = new char[l + 1];
	for (int i = 0; i < l; i++)
		buffer[i] = deCrypt(str[i]);
	buffer[l] = '\0';
	return buffer;
}
int PermutaCypher::findCipherIndex(char x) {
	for (int i = 0; i < 26; i++)
		if (cipherTable[i] == x)
			return i;
	return -1;
}
bool PermutaCypher::findDuplicate(char x,int l) {
	for (int i = 0; i < l; i++)
		if (cipherTable[i] == x) {
			return true;
		}
	return false;
}
int findAlphaIndex(char x) {
	for (int i = 0; i < 26; i++)
		if (alphaTable[i] == x)
			return i;
	return -1;
}
