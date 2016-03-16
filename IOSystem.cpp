//Ho Choi 53734155
#include "IOSystem.h"

using namespace std;
IOSystem::IOSystem()
{
	ldisk = new char*[64];
	for (int i = 0; i < 64; i++)
		ldisk[i] = new char[64];
}
IOSystem::~IOSystem()
{
	delete[] ldisk;
}

void IOSystem::read_block(int i, char* p)
{

	try
	{
		for (int k = 0; k < 64; k++)
		{
			*(p+k) = ldisk[i][k];
		}
		
	}
	catch (exception e)
	{
		cout << "error::read_block::???" << endl;
	}
}
void IOSystem::write_block(int i, char* p)
{
	try
	{
		for (int k = 0; k < 64; k++)
		{
			ldisk[i][k] = *(p+k);
		}

	}
	catch (exception e)
	{
		cout << "error::read_block::???" << endl;
	}
}
