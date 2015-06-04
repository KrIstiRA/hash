#pragma once
#include "abstractHash.h"
#include <iomanip>
#include <sstream>
#include <string.h>
#include <stdio.h>
class gost341194 :
	public abstractHash
{
public:
	char* hexdigest(bytes m);

private:
	bytes f(bytes Hin, bytes m);
	vector<bytes> getKeys(bytes U, bytes V);
	bytes A(bytes Y);
	bytes P(bytes Y);
	bytes E(bytes D, bytes K);
	bytes E_step(bytes A, bytes K);
	bytes psi(bytes Y, int n);
	bytes sum32(bytes sum, bytes m);
	bytes my_xor(bytes a, bytes b);
};


char* gost341194::hexdigest(bytes m)
{
	bytes h(32);
	bytes sum(32);
	bytes L(32);

	for (int i = 0; i < (int)m.size() - 31; i += 32)
	{
		bytes chunk = this->_slice(m, i, i + 32);
		sum = sum32(sum, chunk);
		h = f(h, chunk);
	}
	L[m.size() / 32] = 1;

	if (m.size() % 32)
	{
		L[0] = (m.size() % 32) * 8;

		int len = m.size();
		bytes chunk = this->_slice(m, len - len%32, len);
		chunk.insert(chunk.end(), 32 - len % 32, 0);
		h = f(h, chunk);
		sum = sum32(sum, chunk);
	}

	h = f(h, L);
	h = f(h, sum);

	ostringstream res;
	res.fill('0');
	for (int i = 0; i < 32; i++)
	{
		res << std::hex << setw(2) << (int)h[i];
	}

	char* result = new char[65];
	strcpy(result, (char*)res.str().c_str());
	return result;
}

bytes gost341194::f(bytes Hin, bytes m)
{
	vector< bytes > keys = getKeys(Hin, m);

	bytes S;
	for (int i = 0; i < 4; i++)
	{
		bytes tmp = this->_slice(Hin, i * 8, (i + 1) * 8);
		tmp = E(tmp, keys[i]);
		S.insert(S.end(), tmp.begin(), tmp.end());
	}

	bytes res = psi(S, 12);
	res = my_xor(m, res);
	res = psi(res, 1);
	res = my_xor(Hin, res);
	return psi(res, 61);
}

vector<bytes> gost341194::getKeys(bytes U, bytes V)
{
	unsigned char C3_[] = {
		0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff,
		0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00,
		0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff,
		0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff };
	bytes C3(C3_, C3_ + sizeof(C3_) / sizeof(C3_[0]));

	vector<bytes> K(4);
	bytes W = my_xor(U, V);
	K[0] = P(W);
	for (int j = 1; j < 4; j++)
	{
		U = A(U);
		if (j == 2)
			U = my_xor(U, C3);
		V = A(A(V));
		W = my_xor(U, V);
		K[j] = P(W);
	}

	return K;
}

bytes gost341194::A(bytes Y)
{
	bytes res(32);
	for (int i = 0; i < 24; i++)
		res[i] = Y[i + 8];

	for (int i = 0; i < 8; i++)
		res[i + 24] = Y[i] ^ Y[i + 8];

	return res;
}

bytes gost341194::P(bytes Y)
{
	bytes res(32);
	for (int i = 0; i <= 3; i++)
		for (int k = 1; k <= 8; k++)
			res[i + 1 + 4 * (k - 1) - 1] = Y[8 * i + k - 1];

	return res;
}

bytes gost341194::E(bytes D, bytes K)
{
	vector<bytes> keys;
	for (int i = 0; i < 8; i++)
	{
		bytes tmp = this->_slice(K, i * 4, (i + 1) * 4);
		keys.push_back(tmp);
	}

	bytes A = _slice(D, 0, 4);
	bytes B = _slice(D, 4, 8);

	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			bytes tmp = E_step(A, keys[i]);
			tmp = my_xor(tmp, B);
			B = A;
			A = tmp;
		}
	}

	for (int i = 7; i >= 0; i--)
	{
		bytes tmp = E_step(A, keys[i]);
		tmp = my_xor(tmp, B);
		B = A;
		A = tmp;
	}

	bytes res;
	res.insert(res.end(), B.begin(), B.end());
	res.insert(res.end(), A.begin(), A.end());

	return res;
}

bytes gost341194::E_step(bytes A, bytes K)
{
	unsigned char S[8][16] = {
		{ 4, 10, 9, 2, 13, 8, 0, 14, 6, 11, 1, 12, 7, 15, 5, 3 },
		{ 14, 11, 4, 12, 6, 13, 15, 10, 2, 3, 8, 1, 0, 7, 5, 9 },
		{ 5, 8, 1, 13, 10, 3, 4, 2, 14, 15, 12, 7, 6, 0, 9, 11 },
		{ 7, 13, 10, 1, 0, 8, 9, 15, 14, 4, 6, 12, 11, 2, 5, 3 },
		{ 6, 12, 7, 1, 5, 15, 13, 8, 4, 10, 9, 14, 0, 3, 11, 2 },
		{ 4, 11, 10, 0, 7, 2, 1, 13, 3, 6, 8, 5, 9, 12, 15, 14 },
		{ 13, 11, 4, 1, 3, 15, 5, 9, 0, 10, 14, 7, 6, 8, 2, 12 },
		{ 1, 15, 13, 0, 5, 7, 10, 4, 9, 2, 3, 14, 6, 11, 8, 12 },
	};

	bytes res(4);
	int c = 0;
	for (int i = 0; i < 4; i++)
	{
		c += A[i] + K[i];
		res[i] = c & 0xFF;
		c >>= 8;
	}

	for (int i = 0; i < 8; i++)
	{
		int x = res[i >> 1] & ((i & 1) ? 0xF0 : 0x0F);

		res[i >> 1] ^= x;
		x >>= (i & 1) ? 4 : 0;
		x = S[i][x];
		res[i >> 1] |= x << ((i & 1) ? 4 : 0);
	}

	int tmp = res[3];
	res[3] = res[2];
	res[2] = res[1];
	res[1] = res[0];
	res[0] = tmp;
	tmp = res[0] >> 5;

	for (int i = 1; i < 4; i++)
	{
		int nTmp = res[i] >> 5;
		res[i] = (res[i] << 3) | tmp;
		tmp = nTmp;
	}

	res[0] = (res[0] << 3) | tmp;
	return res;
}

bytes gost341194::psi(bytes Y, int n)
{
	for (int i = 0; i < n; i++)
	{
		unsigned char tmp[] = { 0, 0 };

		char indexes[] = { 1, 2, 3, 4, 13, 16 };
		for (int j = 0; j < sizeof(indexes); j++)
		{
			tmp[0] ^= Y[2 * (indexes[j] - 1)];
			tmp[1] ^= Y[2 * (indexes[j] - 1) + 1];
		}

		for (int i = 0; i < 30; i++)
			Y[i] = Y[i + 2];

		Y[30] = tmp[0];
		Y[31] = tmp[1];
	}
	return Y;
}

bytes gost341194::sum32(bytes sum, bytes m)
{
	int c = 0;
	for (int i = 0; i < 32; i++)
	{
		c += sum[i] + m[i];
		sum[i] = c & 0xFF;
		c >>= 8;
	}
	return sum;
}

bytes gost341194::my_xor(bytes a, bytes b)
{
	for (size_t i = 0; i < a.size(); i++)
		a[i] ^= b[i];
	return a;
}
