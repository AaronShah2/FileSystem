//Ho Choi 53734155
//cmarch314@gmail.com
#include "FileSystem.h"
using namespace std;
string location = "c:/";
char null = '\0';

FileSystem::FileSystem()
{
	ldisk = new IOSystem();
	buffer = new char[64];
	mask = new unsigned int[32];
	oft = new OFT[4];
	descriptor = new int[4];

	////    default  activity     /////
	///////////fill disk///////////////
	for (int i = 0; i < 64; i++)	///
	{								///
		buffer[i] = '\0';			///
	}								///
	for (int i = 0; i < 64; i++)	///
	{								///
		ldisk->write_block(i,buffer);//
	}								///
	////32 bit map mask generating//// mask[0]  = 1000 0000 ... 0000 0000
									// mask[1]  = 0100 0000 ... 0000 0000
	for (int i = 31; i >= 0; i--)	// mask[2]  = 0010 0000 ... 0000 0000
	{								// mask[30] = 0000 0000 ... 0000 0010
		mask[i] = 1 << (31-i);		// mask[31] = 0000 0000 ... 0000 0001
									//to check :: cout << bitset<32>(mask[i]) << endl;
	}								// 
	////////////////////////////////// * mask is an array of unsigned integer that holds a bit of corresponding position.
	
	init();
};

FileSystem::~FileSystem()
{
	delete this;
};

string FileSystem::init() // default init. (all reset) //in 
{								
	////initiate bitmap///////32 bit system///////////////////////////////	bitmap
	((unsigned int*)buffer)[0] = mask[0];//system reserved bitmap		//	buffer on.
	// 6 blocks for file descriptor.									//
	((unsigned int*)buffer)[0] = ((unsigned int*)buffer)[0] | mask[1] |	//
		mask[2] | mask[3] | mask[4] | mask[5] | mask[6];				//
	//set 0 for rest of bits											//
	((unsigned int*)buffer)[1] = mask[1] & mask[2];						//
	//write initial bitmap into ldisk									//
	ldisk->write_block(0, buffer);										//	buffer off.
	//////////////////////////////////////////////////////////////////////

	//show_bitmap();//--------------------------------------------------------------------------check initial bitmap

	////create initial discriptor, (with directory)	//////						descriptor
	descriptor[0] = 0; //size							//
	descriptor[1] = -1; //initial block					//
	descriptor[2] = -1;//not assigned yet				//
	descriptor[3] = -1;//								// 
	set_desc(0);			// descriptor[0] = root		// buffer on, buffer off.
														// set default descriptor
	descriptor[0] = -1; //size							//
	descriptor[1] = -1; //initial block					//
	descriptor[2] = -1;//not assigned yet				//
	descriptor[3] = -1;//								// 
	for (int i = 1; i < 24; i++)						//
	{													//
		set_desc(i);									//
	}													//
	//////////////////////////////////////////////////////

	//////////////reserve OFT/////////////////////								OFT
	for (int i = 0; i < 4; i++)					//	
	{											//
		oft[i].init();							//
	}											//
	//////////////////////////////////////////////

	//open default dir file.//////////////////									directory
	oft[0].init(0, 0);						// //oft[0] = oft(desc,len) = oft(0,0)
	ldisk->read_block(7, oft[0].get_buf());	// //oft_buffer = ldisk[7]
	//////////////////////////////////////////

	return "disk initialized";
}	

//load ldisk, or create new(if not exists)
string FileSystem::init(string* txt_name)	//try to open a file if fails run default init.
{
	string path = location;
	char dummy[1];
	ifstream ifile(path+(*txt_name), std::ifstream::in);
	try
	{
		if (ifile) // load successful  
		{
			//////////////loading////////////////// buffer in						bitmap, descriptor, 
			for (int i = 0; i < 64; i++)		/// for each block
			{									///
				for (int j = 0; j < 64; j++)	/// for each byte
				{								///
					ifile.read(buffer+j,1);		/// read into buffer
					if (j % 4 == 3)				/// 
						ifile.read(dummy,1);	/// erase white space
				}								///
				ifile.read(dummy, 1);			/// erase nextline
				ldisk->write_block(i, buffer);	/// write it into ldisk buffer out
			}									///
			///////////////////////////////////////

			ifile.close();						/// no need file any more.

			//////////////reserve OFT/////////////////////								OFT
			for (int i = 0; i < 4; i++)					//	
			{											//
				oft[i].init();							//
			}											//
			//////////////////////////////////////////////


			///////////////////set directory//////////////									directory
			descriptor = get_desc(0);					//
			oft[0].init(0, descriptor[0]);				//	oft[0].length = root.length
			ldisk->read_block(descriptor[1], oft[0].get_buf());	//oft[0]buffer = ldisk[7]
			//////////////////////////////////////////////

			return "disk restored";
		}
	}
	catch (...)
	{
	}



	return init();
};

//save ldisk
string FileSystem::save(string* txt_name)
{
	//////////////close(3~0)//////////////////
	string path = location;					//
	for (int i = 3; i >= 0; i--)			//
	{										//
		if (oft[i].get_desc() != -1)		//
		{									//
			close(i);						//
		}									//
	}										//
	//////////////////////////////////////////

	////////////////file process//////////////////////
	ofstream ofile(path+(*txt_name), std::ofstream::out);
	if (ofile)										// read successful
	{
		for (int i = 0; i < 64; i++)				//
		{
			ldisk->read_block(i, buffer);			//
			for (int j = 0; j < 64; j++)
			{										//
				ofile << buffer[j];
				if (j % 4 == 3)						//
					ofile << " ";
			}										//
			ofile<<endl;
		}											//
		ofile.close();
		init();
		return "disk saved";						//
	}
	return "save process failed";
	/////////////////////////////////////////////////
	
};


void FileSystem::create(char* file_name)//cr , allocates descriptor, directory.
{
	int desc_n;
	int bitmap_n;

	//name already exists?
	if (search_dir(file_name) != oft[0].get_length())	// oft[0] buffer = searched result
	{	throw exception("create::file already exists");	}
	desc_n = alloc_desc();
	//Descriptor available?
	if (desc_n == -1)//discriptor full
	{
		throw exception("create::all desc taken");
	}

	alloc_dir();
	for (int i = 0; i < 4; i++)	//write the name
	{
		write(0, &(file_name[i]), 1);
	}
	for (int i = 0; i < 4; i++)	//write descriptor number
	{
		write(0, &(((char*)&desc_n)[i]), 1);
	}
	//////////Generate descriptor/////////////
	descriptor[0] = 0;						//length: 0
	descriptor[1] = -1;						//null 
	descriptor[2] = -1;						//null
	descriptor[3] = -1;						//null
	//////////////////////////////////////////
	//															descriptor
	set_desc(desc_n);			//buffer in, buffer out	

};

bool FileSystem::destroy(char* file_name)
{
	int desc_n;
	int bitmap_n;

	//check directory
	lseek(0, 0);
	if (search_dir(file_name) == oft[0].get_length())
	{
		throw exception("destroy::file_not exist");
	}


	//get descriptor from directory, (based on search_dir)
	//									buffer in
	read(0, buffer, 8);
	desc_n = (int)(((int*)buffer)[1]);
	//									buffer out
	descriptor = get_desc(desc_n);	//buffer in buffer out.

	//close it first//////////////////////////////
	for (int i = 1; i < 4; i++)					//
	{											//
		if (oft[i].get_desc() == desc_n)		//
		{										//
			close(i);							//
			break;								//
		}										//
	}											//
	//////////////////////////////////////////////
	//release bitmap
	ldisk->read_block(0, buffer);								//buffer in
	for (int i = 1; i < 4; i++) // descriptor [1], [2], [3]
	{
		bitmap_n = descriptor[i];
		if (bitmap_n != -1)
		{
			((unsigned int*)buffer)[bitmap_n / 32] = ((unsigned int*)buffer)[bitmap_n / 32] & ~mask[bitmap_n%32];
		}
		descriptor[i] = -1;
	}
	ldisk->write_block(0, buffer);								//buffer out
	//update descriptor
	descriptor[0] = -1;//size = -1 meaning not assigned
	set_desc(desc_n);
	search_dir(file_name);
	//remove dir
	write(0, &null, 8);
	return true;
};//de 

//return OFT index
int FileSystem::open(char* file_name)// op 
{
	int desc_n;
	//check file exists
	lseek(0, 0);
	int position = search_dir(file_name); 
	if (position == oft[0].get_length())
	{	throw exception("open::file_not exist");	}
	read(0, buffer, 8);								//buffer in
	desc_n = ((int*)buffer)[1];
	descriptor = get_desc(desc_n);	
	//check already opened.
	for (int i = 1; i < 4; i++)
	{
		if (oft[i].get_desc() == ((int*)buffer)[0])
		{	throw exception("open::already opened");		}
	}

	//check available
	for (int i = 1; i < 4; i++)
	{
		if (oft[i].get_desc() == -1)		//empty slot
		{
			oft[i].init(desc_n,descriptor[0]); // oft(desc_n, file length)
			// no need to read initial buffer, read will take care of it.
			//			ldisk->read_block(descriptor[1], oft[i].get_buf());//read first block on the buffer
			return i; //complete!
		}
	}
	throw exception("open::OFT full");
};

//uses descriptor
int FileSystem::close(int index)//cl 
{
	if (index > 3 || index < 0)
	{	throw exception("close::index out of bound"); }
	
	int desc_n, slot, current_block, report;
	if (oft[index].get_desc() != -1) // if exists
	{
		desc_n = oft[index].get_desc();
		get_desc(desc_n);

		//save desc length 
		((int*)descriptor)[0] = oft[index].get_length();
		oft[index].get_length();
		set_desc(desc_n);

		// save current to ldisk
		slot = oft[index].get_slot();
		report = oft[index].status();
		if (report == 1) // file is empty, no block assigned
		{
			oft[index].init();
			return index;
		}
		else if (report > 1||report < -1) // file was ready to go next block // save the one before.
		{
			slot = slot - 1;
			ldisk->write_block(descriptor[slot], oft[index].get_buf());
		}
		else // save current block. (report == 0);
		{
			ldisk->write_block(descriptor[slot], oft[index].get_buf());
		}
		//release oft
		oft[index].init();
		return index;
	}
	else
	{
		throw exception("close::nothing to close");
	}
};

//return rest of counts.
int FileSystem::read(int index, char* mem_area, int count)//rd 
{
	int i= 0;
	if (index > 3||index < 0)
	{	throw exception("read::oft index out of bound");	}
	if (count < 0)
	{	throw exception("read::count cannot be negative");	}
	int report = oft[index].status();		
	int desc_n = oft[index].get_desc();
	descriptor = get_desc(desc_n);
	while ((report <= 0) && count > 0 && (oft[index].get_cur_pos() < oft[index].get_length()))//readable in bound
	{
		if(report < 0)//get ready to read next block
		{
			ldisk->read_block(descriptor[(-1)*report], oft[index].get_buf()); //advance next block.
		}
		report = oft[index].readc(&mem_area[i]);
		i++; count--;
	}
	return count;	//report how many char read
};

//return #bytes written
int FileSystem::write(int index, char* mem_area, int count)//wr 
{
	if (index > 3 || index < 0)
	{		throw exception("write::index out of bound");	}
	if (count < 0)
	{	throw exception("write::cannot write negative amount");	}
	if (oft[index].get_desc() == -1)
	{	throw exception("write::file not opened"); }
	int report = oft[index].status();
	int desc_n = oft[index].get_desc();
	descriptor = get_desc(desc_n);					//buffer in, buffer out
	int bitmap_n = descriptor[oft[index].get_slot()];

	while((report != 4) && count > 0)//until max cap
	{
		if (report < 0)// readable next block reached.
		{
			if (report < -1)
				ldisk->write_block(descriptor[(-1)*(report) - 1], oft[index].get_buf()); //save the one before
			ldisk->read_block(descriptor[(-1)*report], oft[index].get_buf()); //advance next block.
		}
		else if (report > 0)//writable block
		{//save current first.
			if (report > 1 && report !=5)
				ldisk->write_block(descriptor[report - 1], oft[index].get_buf()); //save the block before proceed.
			if (report != 4)//allocate new block 
			{
				bitmap_n = alloc_bitmap();	//get new bitmap_n
				if (bitmap_n == -1)			//all blocks taken
				{
					return count;
				}
				ldisk->read_block(0, buffer);						//buffer in
				((unsigned int*)buffer)[bitmap_n / 32] = ((unsigned int*)buffer)[bitmap_n / 32] | mask[bitmap_n % 32];
				ldisk->write_block(0, buffer);						//buffer out
				descriptor[report] = bitmap_n;//[1], [2] or [3] will get new bitmap number
				descriptor[0] = oft[index].get_cur_pos(); //update current size.
				set_desc(desc_n);//update descriptor, buffer in, buffer out.
				oft[index].flush();//clear buffer before write.
			}
			else
			{
				return count;
			}
		}
		report = oft[index].writec(mem_area);
		count--;
	}

	return count;	//report how many char written
};

int FileSystem::lseek(int index, int pos) // save current, load pos. check not allowed pos.
{
	if (index > 3 || index < 0)
	{	throw exception("lseek::index out of bound");}
	if (oft[index].get_desc() == -1)
	{	throw exception("lseek::not opened");}
	if (oft[index].get_length()+1 < pos || pos < 0 || pos == 64*3) //you can go size + 1 but not allowed to read, and not allowed to go 4th block.
	{	throw exception("lseek::out of index"); }	
	int desc_n = oft[index].get_desc();
	
	get_desc(desc_n);
	int slot = oft[index].get_slot();
	int report = oft[index].status();

	//save current data
	if (report == 0) // safe bound
	{
		ldisk->write_block(descriptor[slot], oft[index].get_buf());
	}
	else// advance level. so save the one before it.
	{
		if (report != 1 && report != -1) //
			ldisk->write_block(descriptor[slot-1], oft[index].get_buf());
	}
	oft[index].seek(pos);
	report = oft[index].status();
	slot = oft[index].get_slot();
	if (report <= 0)//if read able
	{
		ldisk->read_block(descriptor[slot], oft[index].get_buf());
	}
	return pos;
};

string FileSystem::directory() // dr 
{
	lseek(0, 0);
	int desc_n = oft[0].get_desc();
	descriptor = get_desc(desc_n);
	int length = oft[0].get_length();
	if (length == 0)
	{	return "";	}
	int number_of_file = (length / 4) / 2; // ((int)(int))
	string result = "";
	char* name = new char[5];
	char* skip = new char[5];
	skip[4] = '\0';

	for (int i = 0; i < number_of_file; i++)
	{
		name[4] = '\0';
		read(0, name, 4);
		if (name[0] != '\0')
		{
			result.append(string(name));
			result.append(" ");
		}
		read(0, name, 4);
	}
	return result;
};

//find empty bitmap spot, full:-1
int FileSystem::alloc_bitmap()
{
	int test = 1;
	//find empty block using bitmap
	ldisk->read_block(0, buffer);
	for (int i = 0; i < 2; i++) //currently bitmap is two block
	{
		for (int j = 0; j < 32; j++)
		{
			test = (((unsigned int*)buffer)[i] & mask[j]);
			if (test == 0)
			{
				return i * 32 + j;
			}
		}
	}
	return -1;//not available.
}

//find empty desc, full:-1
int FileSystem::alloc_desc()
{
	for (int i = 0; i < 6; i++)
	{
		ldisk->read_block(i + 1, buffer);
		for (int j = 0; j < 4; j++)
		{
			if (((int*)buffer)[j * 4] == -1 )//find empty desc
			{
				return ((i)* 4) + j;		// [0][1][2][3],	: ldisk[1]
			}								// [4][5][6][7],	: ldisk[2]
		}									//		...		
	}										//[20][21][22][23]	: ldisk[6]
	return -1;//full
}

//return located empty spot, full:-1
int FileSystem::alloc_dir() //al dr
{
	int report;
	char temp[4] = { '\0', '\0', '\0', '\0' };
	
	lseek(0, 0);
	search_dir(temp);

	if(oft[0].get_cur_pos() == 64*3 )
	{  //dir full.
		return -1;
	}
	return oft[0].get_cur_pos();//found means we need to know position?
}
//find dir, return position of desc.
int FileSystem::search_dir(char* file_name)
{
	lseek(0, 0);
	char temp[8];
	int report = oft[0].status();
	while (oft[0].get_cur_pos() < oft[0].get_length())
	{
		read(0, temp, 8);
		if ((int)*((int*)temp) == (int)*((int*)file_name))//name match
		{
			lseek(0, oft[0].get_cur_pos() - 8); //aim the position
			return oft[0].get_cur_pos(); //return the position.
		}

	}
	
	return oft[0].get_cur_pos();
}
void FileSystem::show_bitmap()
{
	char temp[64];
	ldisk->read_block(0, temp);
	cout << "#123456D|..12...|..20...|..28... |..36...|..44...|..52...|..60..." << endl;
	cout << bitset<32>(((unsigned int*)temp)[0]) << " ";
	cout << bitset<32>(((unsigned int*)temp)[1]) << endl;
}
void FileSystem::show_desc()
{
	for (int i = 0; i < 6; i++)				// [0][1][2][3],	: ldisk[1]
	{										// [4][5][6][7],	: ldisk[2]
		ldisk->read_block(i + 1, buffer);	//		...
		for (int j = 0; j < 4; j++)			//[20][21][22][23]	: ldisk[6]
		{
			cout << "[";
			for (int k = 0; k < 4; k++)
			{
				cout << ((int*)buffer)[k+j*4] << ",";
			}
			cout << "]";
		}						
		cout << endl;
	}										
}
int* FileSystem::get_desc(int d)
{
	ldisk->read_block( 1 + ( d / 4 ) ,buffer);
	for (int i = 0; i < 4; i++)
	{
		descriptor[i] = ((int*)buffer)[ i + (d%4) * 4 ];//ex 0 + 0(8%4) * 4 = 0, 1, 2, 3
	}
	return descriptor;
}
int FileSystem::set_desc(int d)
{
	ldisk->read_block(1 + (d / 4), buffer); // ex, 1 + 0/4 = 1 : ldisk[1]
	for (int i = 0; i < 4; i++)
	{
		((int*)buffer)[i + (d % 4) * 4] = descriptor[i];
	}
	ldisk->write_block(1 + (d / 4), buffer);
	return 0;
}
