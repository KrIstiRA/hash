#include "sha1.h"
#include <iostream>
#include <string.h>

using namespace std;

void printUsage()
{
	cout << "Usage:" << endl;
    cout << "   hash <file name>" << endl;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
	{
		printUsage();
		return 1;
	}

    sha1 s;

    char* filename = argv[1];
    try
    {
        cout << s.hexdigestFile(filename) << " " << filename << endl;
	
    }
		catch (int err)
		{
			if (err == BAD_FILE)
				cerr << "Can't calculate hash for file " << filename << endl;
			return 1;
			
		}

	return 0;
}
