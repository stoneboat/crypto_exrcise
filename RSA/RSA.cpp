#include <iostream>
#include "RSA.h"
#include "BigNum.h"
using std::cout;
using std::endl;
BigNum RSA::Cipher(BigNum m) {
	return FastPower(BigNum(m), Pub_Key.e, Pub_Key.n);
}
BigNum RSA::DeCipher(const BigNum& c) {
	return FastPower(c, Pri_Key.d, Pri_Key.n);
}
int main() {
	BigNum p("B149D88EE2533422B612BBD11D7748C9FE5D4ADDF06659C45209FF941B56834C1927ABCAC5B2CAE815552A256E789ADDC2D74C17B4277405959CEE0C2778516D");
	BigNum q("8E5EC441AF9B791E942DE5E87BCE99F1AE9649C3D5F8CF7340EC6DBFED662F8700A75009696F1A3D55088A57628209818144DDEFC75A2CB280CA961C3E3DBC11");
	BigNum m(19);
	RSA x(p, q);
	BigNum c= x.Cipher(m);
	m = x.DeCipher(c);
	char test[129];
	test[128] = '\0';
	m.print(test, 128);
	cout << test << endl;
	return 0;
}