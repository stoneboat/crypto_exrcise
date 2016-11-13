#pragma once
/*Permutation cipher class
@parm cypherTable adopted the method of permutate @parm key and put ordering by rest alphaTable
      note key must end by terminator '\0'
Construct by TransCypher(int key)
char* deCrypt(char*, int) and char* Crypt(char*, int) return value is an char* must be delete automately
@auxiliary function findAlphaIndex£¨char x) return x's index in alphaTable,x must Upper
*/
const char alphaTable[26] = { 'A',  'B',  'C',  'D' ,'E' ,'F', 'G', 'H' ,'I', 'J' , 'K', 'L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
class PermutaCypher {
public:
	PermutaCypher(char* key);
	char Crypt(char);
	char deCrypt(char);
	char* Crypt(char*, int);
	char* deCrypt(char*, int);
private:
	char key[1000];
	char cipherTable[26];
	int findCipherIndex(char x);
	bool findDuplicate(char x,int l);
};
int findAlphaIndex(char x);