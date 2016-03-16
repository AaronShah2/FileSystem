//Ho Choi 53734155
#include <stdlib.h>
#include "OFT.h"
using namespace std;

OFT::OFT()
{
	oft_buffer = new char[64];
	cur_pos = -1;
	desc = -1;
	length = -1;
}
OFT::~OFT()
{
	delete this;
}
void OFT::init()//release.
{
	cur_pos = -1;
	desc = -1;
	length = -1;
	for (int i = 0; i < 64; i++)
	{
		oft_buffer[i] = '\0';
	}
}
void OFT::init(int d,int l)//assign.
{
	flush();
	cur_pos = 0;
	desc = d;
	length = l;
}

int OFT::readc(char* c)
{
	*c = oft_buffer[(cur_pos) % 64];
	cur_pos++;
	return status(); // readc -> b[pos] = c, size = pos. 0,0. 1,1 ... 
}

int OFT::writec(char* c)
{
	oft_buffer[(cur_pos) % 64] = *c;
	(cur_pos)++;
	if ((cur_pos) == (length)+1)
	{
		(length)++;
	}
	return status(); // return what is going to be next.
}

int OFT::seek(int index)
{
	if (index <= length && index >= 0)
	{
		cur_pos = index;
		return 0;
	}
	return -1;
}

char* OFT::get_buf()
{
	return oft_buffer;
}
int OFT::get_cur_pos()
{
	return cur_pos;
}
int OFT::get_desc()
{
	return desc;
}
int OFT::get_length()
{
	return length;
}
int OFT::get_slot() //slot means shat is going to be next slot for usage.
{
	int slot = ((cur_pos) / 64) + 1; // so 0 ~ 63 : 1st slot, 64 ~ 127 = 2nd slot, 128~ 195 = last slot.
	return slot;
}
// 4 means do nothing. 3:load 3rd slot. 
int OFT::status()
{
	if (cur_pos == 192)
	{
		return 4;//max can't write, can't read anymore.
	}
	else if (cur_pos == 128 && cur_pos == length)// to write slot 3
	{
		return 3;
	}
	else if (cur_pos == 64 && cur_pos == length) // to write slot 2
	{
		return 2;
	}
	else if (cur_pos == 0 && cur_pos == length) // not available.
	{
		return 1; // need to write first block. 
	}
	else if (cur_pos == 128 && cur_pos < length) // to read slot 3
	{
		return -3;
	}
	else if (cur_pos == 64 && cur_pos < length) // to read slot 2
	{
		return -2;//
	}
	else if (cur_pos == 0 && cur_pos < length)
	{
		return -1; // to read first block.
	}
	else
	{
		return 0; // safe, normal state. write or read what ever. 
	}
}
void OFT::flush()
{
	for (int i = 0; i < 64; i++)
	{
		oft_buffer[i] = '\0';
	}
}
/*
cur pos = 0 ~ 63
length = 0 ~ 63
*/