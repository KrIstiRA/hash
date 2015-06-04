#define _CRT_SECURE_NO_WARNINGS
#include "abstractHash.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string.h>

using namespace std;

// вытаскивает данные из файла и подает на вход функции hexdigest
char* abstractHash::hexdigestFile(const char* fileName)
{
	ifstream file(fileName);
	if (file.fail())
		throw BAD_FILE;

	file.seekg(0, ios::end);
	size_t SizeOfFile = file.tellg();
	file.seekg(0, ios::beg);

	bytes content;
	if (SizeOfFile)
	{
		content.resize(SizeOfFile);
		file.read((char*)&content.front(), SizeOfFile);
		file.close();
	}

	return this->hexdigest(content);
}

// подготавливает строку для вызова функции hexdigest
char* abstractHash::hexdigestString(const char* str)
{
	unsigned char* buffer = (unsigned char*)str;
	bytes msg(buffer, buffer + strlen(str));
	return this->hexdigest(msg);
}

// формирование печатного вида хеша. 
char* abstractHash::_printHash(dwords sum, bool bigEndian)
{
	// выведем результат в string-поток, потом достанем из него char*
	ostringstream outStream;
	outStream.fill('0');
	for (size_t i = 0; i < sum.size(); i++)
	{
		if (bigEndian)
			outStream << hex << setw(8) << sum[i];
		else
			outStream << hex << setw(8) << this->_bigToLittle(sum[i]);
	}

	// 8 = (2 символа на один байт) * (4 байта в dword);
	// 1 ~ '\0'
	char* result = new char[(sum.size() * 8 + 1)*2];
	strcpy(result, (char*)outStream.str().c_str());
	return result;
}

dword abstractHash::_bigToLittle(dword bigEndian)
{
	dword littleEndian = 0;
	for (int i = 0; i < 32; i += 8)
	{
		littleEndian |= ((bigEndian >> i) & 0xFF) << (24 - i);
	}

	return littleEndian;
}

dwords abstractHash::_bytesToDwords(bytes in, bool bigEndian)
{
	dwords result;
	for (size_t i = 0; i < in.size(); i += 4)
	{
		dword d = 0;
		for (int j = 0; j < 4; j++)
		{
			if (bigEndian)
				d |= in[i + j] << (3-j) * 8;
			else
				d |= in[i + j] << j * 8;
		}
		result.push_back(d);
	}
	return result;
}

// срез массива двойных слов
// не очень безопасная, но очень удобная функция
dwords abstractHash::_slice(dwords in, int start, int end)
{
	dwords result;
	result.insert(result.begin(), in.begin() + start, in.begin() + end);
	return result;
}

bytes abstractHash::_slice(bytes in, int start, int end)
{
	bytes result;
	result.insert(result.begin(), in.begin() + start, in.begin() + end);
	return result;
}

dword abstractHash::_rotateLeft(dword x, int s)
{
	return (x << s & 0xFFFFFFFF) | (x >> (32 - s) & 0xFFFFFFFF);
}
