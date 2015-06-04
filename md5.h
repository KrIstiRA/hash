// конкретный хеш - md5.
// должен определять хотя бы один метод - hexdigest.
// остальное "оформительство" осталось в базовом классе
#pragma once
#include "abstractHash.h"
class md5 :
	public abstractHash
{
public:
	char* hexdigest(bytes m);

private:
	void _rounds(dwords chunk);
	dword A;
	dword B;
	dword C;
	dword D;
};


char* md5::hexdigest(bytes m)
{
	int L = m.size() * 8; // размер входного сообщения в битах

	
	// Шаг 1. Выравнивание потока
	m.push_back(0x80);
	int zero_bytes = (448 - (L + 8) % 512) / 8; // необходимое число нулевых байт для выравнивания
	if (zero_bytes < 0)
		zero_bytes += 512/8;
	m.insert(m.end(), zero_bytes, 0);

	
	// Шаг 2. Добавление длины сообщения
	for (int i = 0; i < 4; i++)
		m.push_back(L >> i * 8);
	m.insert(m.end(), 4, 0);

	
	// Шаг 3. Инициализация буфера
	A = 0x67452301;
	B = 0xEFCDAB89;
	C = 0x98BADCFE;
	D = 0x10325476;

	// объединяем байты в блоки по 32 бита
	dwords blocks = this->_bytesToDwords(m);

	
	// Шаг 4. Вычисление в цикле
	// объединяем блоки по 16 элементов и прогоняем через 4 раунда каждый
	for (size_t i = 0; i < blocks.size(); i += 16)
	{
		dwords chunk = this->_slice(blocks, i, i + 16);
		this->_rounds(chunk); // преобразовываем ABCD
	}

	
	// Шаг 5. Результат вычислений
	dwords res;
	res.push_back(A);
	res.push_back(B);
	res.push_back(C);
	res.push_back(D);
	return this->_printHash(res);
}

void md5::_rounds(dwords chunk)
{
// определяем функции для 4-х раундов
#define F(x, y, z) ((x & y) | (~x & z))
#define G(x, y, z) ((x & z) | (~z & y))
#define H(x, y, z) (x ^ y ^ z)
#define I(x, y, z) (y ^ (~z | x))

/*[F abcd k s i] : a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define MD5_OPERATOR(F, a, b, c, d, xk, s, t)	a = b + _rotateLeft((a + F(b, c, d) + xk + (dword)t), s)

// Сдвиги для каждого раунда
#define S11 7
#define S12 12
#define S13 17
#define S14 22

#define S21 5
#define S22 9
#define S23 14
#define S24 20

#define S31 4
#define S32 11
#define S33 16
#define S34 23

#define S41 6
#define S42 10
#define S43 15
#define S44 21
	
	dword AA = A;
	dword BB = B;
	dword CC = C;
	dword DD = D;

	MD5_OPERATOR(F, A, B, C, D, chunk[ 0], S11, 0xD76AA478);
	MD5_OPERATOR(F, D, A, B, C, chunk[ 1], S12, 0xE8C7B756);
	MD5_OPERATOR(F, C, D, A, B, chunk[ 2], S13, 0x242070DB);
	MD5_OPERATOR(F, B, C, D, A, chunk[ 3], S14, 0xC1BDCEEE);
	MD5_OPERATOR(F, A, B, C, D, chunk[ 4], S11, 0xF57C0FAF);
	MD5_OPERATOR(F, D, A, B, C, chunk[ 5], S12, 0x4787C62A);
	MD5_OPERATOR(F, C, D, A, B, chunk[ 6], S13, 0xA8304613);
	MD5_OPERATOR(F, B, C, D, A, chunk[ 7], S14, 0xFD469501);
	MD5_OPERATOR(F, A, B, C, D, chunk[ 8], S11, 0x698098D8);
	MD5_OPERATOR(F, D, A, B, C, chunk[ 9], S12, 0x8B44F7AF);
	MD5_OPERATOR(F, C, D, A, B, chunk[10], S13, 0xFFFF5BB1);
	MD5_OPERATOR(F, B, C, D, A, chunk[11], S14, 0x895CD7BE);
	MD5_OPERATOR(F, A, B, C, D, chunk[12], S11, 0x6B901122);
	MD5_OPERATOR(F, D, A, B, C, chunk[13], S12, 0xFD987193);
	MD5_OPERATOR(F, C, D, A, B, chunk[14], S13, 0xA679438E);
	MD5_OPERATOR(F, B, C, D, A, chunk[15], S14, 0x49B40821);

	MD5_OPERATOR(G, A, B, C, D, chunk[ 1], S21, 0xF61E2562);
	MD5_OPERATOR(G, D, A, B, C, chunk[ 6], S22, 0xC040B340);
	MD5_OPERATOR(G, C, D, A, B, chunk[11], S23, 0x265E5A51);
	MD5_OPERATOR(G, B, C, D, A, chunk[ 0], S24, 0xE9B6C7AA);
	MD5_OPERATOR(G, A, B, C, D, chunk[ 5], S21, 0xD62F105D);
	MD5_OPERATOR(G, D, A, B, C, chunk[10], S22, 0x02441453);
	MD5_OPERATOR(G, C, D, A, B, chunk[15], S23, 0xD8A1E681);
	MD5_OPERATOR(G, B, C, D, A, chunk[ 4], S24, 0xE7D3FBC8);
	MD5_OPERATOR(G, A, B, C, D, chunk[ 9], S21, 0x21E1CDE6);
	MD5_OPERATOR(G, D, A, B, C, chunk[14], S22, 0xC33707D6);
	MD5_OPERATOR(G, C, D, A, B, chunk[ 3], S23, 0xF4D50D87);
	MD5_OPERATOR(G, B, C, D, A, chunk[ 8], S24, 0x455A14ED);
	MD5_OPERATOR(G, A, B, C, D, chunk[13], S21, 0xA9E3E905);
	MD5_OPERATOR(G, D, A, B, C, chunk[ 2], S22, 0xFCEFA3F8);
	MD5_OPERATOR(G, C, D, A, B, chunk[ 7], S23, 0x676F02D9);
	MD5_OPERATOR(G, B, C, D, A, chunk[12], S24, 0x8D2A4C8A);

	MD5_OPERATOR(H, A, B, C, D, chunk[ 5], S31, 0xFFFA3942);
	MD5_OPERATOR(H, D, A, B, C, chunk[ 8], S32, 0x8771F681);
	MD5_OPERATOR(H, C, D, A, B, chunk[11], S33, 0x6D9D6122);
	MD5_OPERATOR(H, B, C, D, A, chunk[14], S34, 0xFDE5380C);
	MD5_OPERATOR(H, A, B, C, D, chunk[ 1], S31, 0xA4BEEA44);
	MD5_OPERATOR(H, D, A, B, C, chunk[ 4], S32, 0x4BDECFA9);
	MD5_OPERATOR(H, C, D, A, B, chunk[ 7], S33, 0xF6BB4B60);
	MD5_OPERATOR(H, B, C, D, A, chunk[10], S34, 0xBEBFBC70);
	MD5_OPERATOR(H, A, B, C, D, chunk[13], S31, 0x289B7EC6);
	MD5_OPERATOR(H, D, A, B, C, chunk[ 0], S32, 0xEAA127FA);
	MD5_OPERATOR(H, C, D, A, B, chunk[ 3], S33, 0xD4EF3085);
	MD5_OPERATOR(H, B, C, D, A, chunk[ 6], S34, 0x04881D05);
	MD5_OPERATOR(H, A, B, C, D, chunk[ 9], S31, 0xD9D4D039);
	MD5_OPERATOR(H, D, A, B, C, chunk[12], S32, 0xE6DB99E5);
	MD5_OPERATOR(H, C, D, A, B, chunk[15], S33, 0x1FA27CF8);
	MD5_OPERATOR(H, B, C, D, A, chunk[ 2], S34, 0xC4AC5665);


	MD5_OPERATOR(I, A, B, C, D, chunk[ 0], S41, 0xF4292244);
	MD5_OPERATOR(I, D, A, B, C, chunk[ 7], S42, 0x432AFF97);
	MD5_OPERATOR(I, C, D, A, B, chunk[14], S43, 0xAB9423A7);
	MD5_OPERATOR(I, B, C, D, A, chunk[ 5], S44, 0xFC93A039);
	MD5_OPERATOR(I, A, B, C, D, chunk[12], S41, 0x655B59C3);
	MD5_OPERATOR(I, D, A, B, C, chunk[ 3], S42, 0x8F0CCC92);
	MD5_OPERATOR(I, C, D, A, B, chunk[10], S43, 0xFFEFF47D);
	MD5_OPERATOR(I, B, C, D, A, chunk[ 1], S44, 0x85845DD1);
	MD5_OPERATOR(I, A, B, C, D, chunk[ 8], S41, 0x6FA87E4F);
	MD5_OPERATOR(I, D, A, B, C, chunk[15], S42, 0xFE2CE6E0);
	MD5_OPERATOR(I, C, D, A, B, chunk[ 6], S43, 0xA3014314);
	MD5_OPERATOR(I, B, C, D, A, chunk[13], S44, 0x4E0811A1);
	MD5_OPERATOR(I, A, B, C, D, chunk[ 4], S41, 0xF7537E82);
	MD5_OPERATOR(I, D, A, B, C, chunk[11], S42, 0xBD3AF235);
	MD5_OPERATOR(I, C, D, A, B, chunk[ 2], S43, 0x2AD7D2BB);
	MD5_OPERATOR(I, B, C, D, A, chunk[ 9], S44, 0xEB86D391);

	A = AA + A;
	B = BB + B;
	C = CC + C;
	D = DD + D;
}