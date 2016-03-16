//Ho Choi, OFT confirmed.
#ifndef OFT_H
#define OFT_H
#include <stdlib.h>

using namespace std;
class OFT
{
public:
	
	OFT();
	~OFT();
	
	void init();
	void init(int d,int l);
	int readc(char*);
	int writec(char*);
	int seek(int index);
	char* get_buf();
	int get_cur_pos();
	int get_desc();
	int get_length();
	int get_slot();
	int status();
	void flush();
private:
	int cur_pos;
	int desc;
	int length;
	char* oft_buffer;

};


#endif