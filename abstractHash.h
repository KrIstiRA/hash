// абстрактный класс "хеш"
#pragma once
#include <vector>

using namespace std;

#define BAD_FILE 1
typedef unsigned char byte;
typedef unsigned int dword;
typedef vector<byte> bytes;
typedef vector<dword> dwords;

class abstractHash
{
public:
    
	// посчитать хеш-сумму байт (основная виртуальная абстрактная функция,
	// каждый конкретный хэш должен её определять самостоятельно)
	virtual char* hexdigest(bytes m) = 0;

	// найти хэш-сумму файла. Общая для всех хешей, просто подготавливает данные для 
	// вызова функции hexdigest. Одинакова для всех хешей
	char* hexdigestFile(const char* fileName); 

	// посчитать хеш-сумму текстовой строки.
	// просто подготавливает данные для hexdigest
	char* hexdigestString(const char* str);

protected:
	// подготовка хеш-суммы для вывода
	// практически одинакова для всех хешей
	// либо в BigEndian, либо в LittleEndian
	char* _printHash(dwords sum, bool bigEndian=false);

	// перевод из Big-Endian в Little-Endian
	// также нужна во многих хешах
	dword _bigToLittle(dword bigEndian);

	// перевод массива байт в массив dword
	// либо в BigEndian, либо в LittleEndian
	dwords _bytesToDwords(bytes in, bool bigEndian=false);

	// для удобного "среза" вектора - 
	// возвращает вектор элементов [start...end)
	dwords _slice(dwords in, int start, int end);
    bytes _slice(bytes in, int start, int end);

	// циклический сдвиг влево
	dword _rotateLeft(dword x, int s);
};