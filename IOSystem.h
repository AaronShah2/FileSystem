//Ho Choi 53734155
//cmarch314@gmail.com
#ifndef IOSYSTEM_H
#define IOSYSTEM_H
#include <iostream>
#include <exception>


using namespace std;

class IOSystem
{

public:
	IOSystem();
	~IOSystem();

	void read_block(int i, char* p);
	void write_block(int i, char* p);

private:
	char** ldisk;
	/*
	[0]		[1]		[2]		[3]		[4]		[5]		[6]		[7]		...
	ldisk[0] = 	[   bit map   ]													...
	ldisk[1] = 	[		  descriper0   		]	[		  descriptor1  		]
	ldisk[2] = descriptor 4 ~ 7
	ldisk[3] = descriptor 8 ~ 11
	ldisk[4] = dscrptr 12 ~ 15
	ldisk[5] = dscrptr 16 ~ 19
	ldisk[6] = dscrptr 20 ~ 23

	ldisk[?] = directory 0 ~ 7
	ldisk[?] = directory 8 ~ 15
	ldisk[?] = directory 16 ~ 23

	ldisk[?] = data

	[dcpr0] = subscribes the directory
	directory = [int] 	[int] 					= [filename] [dcpr#] = [int][int]
	descriptor	[int]	[int]	[int]	[int] 	= [file length][block 1][block 2][block 3]
	[4char]	[4char]	[4char]	[4char]
	default unassigned block # = -1,
	*/


};

#endif