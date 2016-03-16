//Ho Choi 53734155
//cmarch314@gmail.com
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

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
#include <io.h>
#include <fcntl.h>
#include "IOSystem.h"
#include <iostream>
#include <exception>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>
#include "OFT.h"
#include <vector>
#include <fstream>
using namespace std;

class FileSystem
{
	
public:
	FileSystem();
	~FileSystem();
	void create(char* filename);
	bool destroy(char* filename);
	int open(char* filename); //return OFT index
	int close(int index);
	int read(int index, char* mem_area, int count); //return #bytes
	int write(int index, char* mem_area, int count); //return #bytes
	int lseek(int index, int pos);
	string directory();
	string init();//create empty file system
	string init(string* filename);//load ldisk, or create new(if not exists)
	string save(string* filename);//save ldisk


	//helper
	void show_bitmap();
	int* get_desc(int d);
	int set_desc(int d);
	void show_desc();

	//find and return available spot
	int alloc_bitmap();
	int alloc_desc();
	int alloc_dir();
	int seek_block(int index,int slot);

	//find and return index of matching result
	int search_dir(char*);

	

private:
	char* buffer;
	IOSystem* ldisk;
	unsigned int* mask;
	OFT* oft;
	int* descriptor;
};

#endif