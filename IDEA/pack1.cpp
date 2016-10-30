#include <iostream>
#include<string>
using std::string;
using std::cout;
using std::endl;
using std::hex;
using std::cin;
/* 求一个 无符号整型的 位数 */
unsigned int getLength(unsigned int x);
/* GF（2） 上的乘法 */
unsigned int doubleBytesTime(unsigned int a, unsigned int b);
/* GF（2^8） 模 m（x) 的结果 */
unsigned int modInGF(unsigned int x, unsigned int m);
/* 多项式a 到 矩阵a */
void mappingPolyTOMatrix(int poly[4], int matrix[4][4]);
# define initial_poly(p,len) {for(int i=0;i<len;i++) p[i] =0;}
int poly_a[4];
int matrix_a[4][4];
int poly_b[4];
int poly_c[4];
int main() {
	/*初始化*/
	initial_poly(poly_a, 4);
	initial_poly(poly_b, 4);
	initial_poly(poly_c, 4);
	for(int i=0;i<4;i++)
		initial_poly(matrix_a[i], 4);
	for (int i = 0; i < 4; i++)
		cin >> hex >> poly_a[i];
	for (int i = 0; i < 4; i++)
		cin >> hex >> poly_b[i];
	mappingPolyTOMatrix(poly_a, matrix_a);
	/*计算*/
	for (int i = 0; i < 4; i++) {
		int count = 0;
		for (int j = 0; j < 4; j++)
		{
			int temp = doubleBytesTime(matrix_a[i][j], poly_b[j]);
			count = count^temp;
		}
		poly_c[i] = modInGF(count, 17);
	}
	for (int i = 0; i < 4; i++)
		cout << hex << poly_c[i] << "  ";
	return 0;
}
void mappingPolyTOMatrix(int poly[4] , int matrix[4][4] ) {
	for (int i = 0; i < 4; i++)
		matrix[i][i] = poly[0];
	matrix[0][1] = matrix[1][2] = matrix[2][3] = matrix[3][0] = poly[3];
	matrix[0][2] = matrix[1][3] = matrix[2][0] = matrix[3][1] = poly[2];
	matrix[0][3] = matrix[1][0] = matrix[2][1] = matrix[3][2] = poly[1];
	/*test*/
	/*for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			cout << matrix[i][j] << " ";
		cout << endl;
	}*/
}
unsigned int doubleBytesTime(unsigned int a, unsigned int b) {
	if (a >> 8 || b >> 8) {
		string message = "illegal format ,just two bytes per operator";
		throw;
	}
	unsigned int result = 0;
	while (b) {
		if (b%2)
			result = result^a;
		b = b >> 1;
		a = a << 1;
	}
	return result;
}
unsigned int getLength(unsigned int x)
{
	unsigned int l_x = 0;
	if (x == 0)
		return 0;
	else
		l_x = 1;
	while (x = x >> 1) 
		++l_x;
	return l_x;
}
unsigned int modInGF(unsigned int x, unsigned int m) {
	if ( m >> 9) {
		string message = "illegal format ,GF(2^8) operation";
		throw;
	}
	unsigned int l_x = getLength(x);
	unsigned int l_m = getLength(m);
	if (l_x < l_m)
		return x;
	int gap = 0;
	while ((gap = l_x-l_m) >= 0 ) {
		unsigned int temp = m << gap;
		x = x^temp;
		l_x = getLength(x);
	}
	return x;
}