#pragma once
#include "abstractHash.h"
class ripemd160 :
	public abstractHash
{
public:
	char* hexdigest(bytes m);

private:
	// Определение действующих функций
	dword f(int j, dword x, dword y, dword z);
	dword K1(int j);
	dword K2(int j);
};


char* ripemd160::hexdigest(bytes m)
{
	int L = m.size() * 8; // размер входного сообщения в битах


	// Шаг 1. Добавление недостающих битов
	m.push_back(0x80);
	int zero_bytes = (448 - (L + 8) % 512) / 8; // необходимое число нулевых байт для выравнивания
	if (zero_bytes < 0)
		zero_bytes += 512 / 8;
	m.insert(m.end(), zero_bytes, 0);


	// Шаг 2. Добавление длины сообщения
	for (int i = 0; i < 4; i++)
		m.push_back(L >> i * 8);
	m.insert(m.end(), 4, 0);

	
	// Шаг 3. Определение действующих функций и констант
	dword R1[] = { 
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
		3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
		1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
		4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13 
	};

	dword R2[] = { 
		5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
		6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
		15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
		8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
		12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11 
	};

	dword S1[] = {
		11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
		7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
		11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
		11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
		9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6 
	};

	dword S2[] = { 
		8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
		9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
		9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
		15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
		8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11 
	};

	dwords h(5);
	h[0] = 0x67452301;
	h[1] = 0xEFCDAB89;
	h[2] = 0x98BADCFE;
	h[3] = 0x10325476;
	h[4] = 0xC3D2E1F0;

	// объединяем байты в блоки по 32 бита
	dwords blocks = this->_bytesToDwords(m);

	
	// Шаг 4. Выполнение алгоритма хеширования
	for (size_t i = 0; i < blocks.size(); i += 16)
	{
		dwords x = this->_slice(blocks, i, i + 16);
		
		dword A1 = h[0];
		dword B1 = h[1];
		dword C1 = h[2];
		dword D1 = h[3];
		dword E1 = h[4];

		dword A2 = h[0];
		dword B2 = h[1];
		dword C2 = h[2];
		dword D2 = h[3];
		dword E2 = h[4];

		unsigned long T;

		for (int j = 0; j < 80; j++)
		{
			T = this->_rotateLeft(A1 + f(j, B1, C1, D1) + x[R1[j]] + K1(j), S1[j]) + E1;
			A1 = E1; 
			E1 = D1; 
			D1 = this->_rotateLeft(C1, 10); 
			C1 = B1; 
			B1 = T;
			
			T = this->_rotateLeft(A2 + f(79 - j, B2, C2, D2) + x[R2[j]] + K2(j), S2[j]) + E2;
			A2 = E2;
			E2 = D2;
			D2 = this->_rotateLeft(C2, 10);
			C2 = B2;
			B2 = T;
		}

		T = h[1] + C1 + D2; 
		h[1] = h[2] + D1 + E2;
		h[2] = h[3] + E1 + A2;
		h[3] = h[4] + A1 + B2; 
		h[4] = h[0] + B1 + C2; 
		h[0] = T;
	}
	return this->_printHash(h);
}

dword ripemd160::f(int j, dword x, dword y, dword z)
{
	dword res;
	if (j >= 0 && j <= 15)
		res = x ^ y ^ z;

	if (j >= 16 && j <= 32)
		res = (x & y) | (~x & z);

	if (j >= 32 && j <= 47)
		res = (x | ~y) ^ z;

	if (j >= 48 && j <= 63)
		res = (x & z) | (y & ~z);

	if (j >= 64 && j <= 79)
		res = x ^ (y | ~z);
	
	return res;
}

dword ripemd160::K1(int j)
{
	dword k1;
	if (j >= 0 && j < 16)
		k1 = 0x00000000;

	if (j >= 16 && j < 32)
		k1 = 0x5A827999;

	if (j >= 32 && j < 48)
		k1 = 0x6ED9EBA1;

	if (j >= 48 && j < 64)
		k1 = 0x8F1BBCDC;

	if (j >= 64 && j < 80)
		k1 = 0xA953FD4E;
	
	return k1;
}

dword ripemd160::K2(int j)
{
	dword k2;
	if (j >= 0 && j < 16)
		k2 = 0x50A28BE6;

	if (j >= 16 && j < 32)
		k2 = 0x5C4DD124;

	if (j >= 32 && j < 48)
		k2 = 0x6D703EF3;

	if (j >= 48 && j < 64)
		k2 = 0x7A6D76E9;

	if (j >= 64 && j < 80)
		k2 = 0x00000000;

	return k2;
}