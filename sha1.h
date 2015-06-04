#pragma once
#include "abstractHash.h"
class sha1 :
	public abstractHash
{
public:
	char* hexdigest(bytes m);
    sha1() { _h = dwords(5); }

private:
    dwords _h;
};


char* sha1::hexdigest(bytes m)
{
    // Инициализация буфера
    // нельзя вынести в конструктор, т. к. эта инициализация должна
    // проводиться перед каждым вычислением хеш-суммы
    this->_h[0] = 0x67452301;
    this->_h[1] = 0xEFCDAB89;
    this->_h[2] = 0x98BADCFE;
    this->_h[3] = 0x10325476;
    this->_h[4] = 0xC3D2E1F0;

	// предварительная обработка
	int L = m.size() * 8;

	// Присоединяем бит '1' к сообщению
	m.push_back(0x80);

	// Присоединяем k битов '0', где k наименьшее число ≥ 0 такое, что длина получившегося сообщения
	// (в битах) сравнима по модулю  512 с 448 (length mod 512 == 448)
	int k8 = (448 - (L + 8) % 512) / 8;
	if (k8 < 0)
		k8 += 512 / 8;
	m.insert(m.end(), k8, 0);

	// Добавляем длину исходного сообщения(до предварительной обработки) как целое 64 - битное
	// Big - endian число, в битах.
	m.insert(m.end(), 4, 0);
	for (int i = 0; i < 4; i++)
		m.push_back(L >> (3-i) * 8);

	// объединяем байты в блоки по 32 бита
	dwords blocks = this->_bytesToDwords(m, true);
	
	// В процессе сообщение разбивается последовательно по 512 бит (64 байта (16 блоков по 4 байта))
	for (size_t i = 0; i < blocks.size(); i += 16)
	{
		// кусок в 512 бит - 16 слов по 32 бита
		dwords w = this->_slice(blocks, i, i + 16);

		// 16 слов по 32 бита дополняются до 80 32-битовых слов:
		for (int i = 16; i < 80; i++)
		{
			//w[i] = (w[i-3] xor w[i-8] xor w[i-14] xor w[i-16]) циклический сдвиг влево 1
			dword wi = this->_rotateLeft(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
			w.push_back(wi);
		}

		//  Инициализация хеш-значений этой части:
        dword a = this->_h[0];
        dword b = this->_h[1];
        dword c = this->_h[2];
        dword d = this->_h[3];
        dword e = this->_h[4];

		// Основной цикл:
		for (int i = 0; i < 80; i++)
		{
			dword f;
			dword k;
			if (i >= 0 && i <= 19)
			{
				f = (b & c) | ((~b) & d);
				k = 0x5A827999;
			}
			if (i >= 20 && i <= 39)
			{
				f = b ^ c ^ d;
				k = 0x6ED9EBA1;
			}
			if (i >= 40 && i <= 59)
			{
				f = (b & c) | (b & d) | (c & d);
				k = 0x8F1BBCDC;
			}
			if (i >= 60 && i <= 79)
			{
				f = b ^ c ^ d;
				k = 0xCA62C1D6;
			}

			dword temp = this->_rotateLeft(a, 5);
			temp = this->_rotateLeft(a, 5) + f + e + k + w[i];
			e = d;
			d = c;
			c = this->_rotateLeft(b, 30);
			b = a;
			a = temp;
		}
		// Добавляем хеш - значение этой части к результату :
        this->_h[0] = this->_h[0] + a;
        this->_h[1] = this->_h[1] + b;
        this->_h[2] = this->_h[2] + c;
        this->_h[3] = this->_h[3] + d;
        this->_h[4] = this->_h[4] + e;
	}
	// Итоговое хеш - значение:
    return this->_printHash(this->_h, true);
}
