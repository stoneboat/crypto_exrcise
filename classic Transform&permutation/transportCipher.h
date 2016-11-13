#pragma once
/*Transportation cypher class
  @parm key right shift alphaTable (key mod 26) port to comform @parm cypherTable
  Construct by TransCypher(int key)
  char* deCrypt(char*, int) and char* Crypt(char*, int) return value is an char* must be delete automately
  @auxiliary function findAlphaIndex£¨char x) return x's index in alphaTable,x must Upper
*/
const char alphaTable[26] = { 'A',  'B',  'C',  'D' ,'E' ,'F', 'G', 'H' ,'I', 'J' , 'K', 'L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
class TransCypher {
public:
	TransCypher(int key);
	char Crypt(char);
	char deCrypt(char);
	char* Crypt(char*, int);
	char* deCrypt(char*, int);
private:
	int key;
	char cipherTable[26];
	int findCipherIndex(char x);
};
int findAlphaIndex(char x);
