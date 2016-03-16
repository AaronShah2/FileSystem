#include "FileSystem.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main()
{
	string location = "h:/";
	FileSystem* FS = new FileSystem();
	string ifname = "";
	string line;
	string command;
	int index;
	int pos;
	int count;
	string disk_cont;
	string name;
	char Char;
	char cname[5];
	char *buffer;
	string out;
	cout << "Enter file name(.txt):";
	getline(cin, ifname);
	ofstream oFile(location +"53734155.txt", std::ofstream::out);
	string::size_type sz; 
	
	ifstream iFile(location + ifname + ".txt", std::ifstream::in);
	while (getline(iFile,line))
	{
		out = "";
		line = line.append(" ");
		try 
		{
			command = line.substr(0, 2);
			if (command.find("cr") == 0)
			{// cr name
				line = line.substr(3);
				if (line.find_first_of(" ") <= 4)
					name = line.substr(0, line.find_first_of(" "));
				else
					name = line.substr(0, 4);
				strcpy_s(cname, name.c_str());
				FS->create(cname);
				oFile << name << " created "<<endl;
			}//name created
			else if (command.find("de") == 0)
			{// de name
				line = line.substr(3);
				if (line.find_first_of(" ") <= 4)
					name = line.substr(0, line.find_first_of(" "));
				else
					name = line.substr(0, 4);
				strcpy_s(cname, name.c_str());
				if(FS->destroy(cname)== true)
					oFile << name << " destroyed " << endl;
			}//name destroyed
			else if (command.find("op") == 0)
			{// op name
				line = line.substr(3);
				if (line.find_first_of(" ") <= 4)
					name = line.substr(0, line.find_first_of(" "));
				else
					name = line.substr(0, 4);
				strcpy_s(cname, name.c_str());
				index = FS->open(cname);
				oFile << name << " opened " << index << endl;
			}// name opened index
			else if (command.find("cl") == 0)
			{// cl index
				line = line.substr(3);
				index = stoi(line, &sz);
				if (index < 1)
				{
					throw exception("cannot close root or negative number");
				}
				index = FS->close(index);
				oFile << index << " closed" << endl;
			}//index closed
			else if (command.find("rd") == 0)
			{// rd index count
				line = line.substr(3);
				index = stoi(line, &sz);
				line = line.substr(sz+1);
				count = stoi(line, &sz);
				int i = 0; buffer = new char[256];
				count = count - FS->read(index, buffer, count);
				while (!(buffer[i] == 'Í' || buffer[i] == NULL))
				{
					oFile<<buffer[i];
					i++;
				}
				oFile<< endl;
			}//xx...xxx
			else if (command.find("wr") == 0)
			{// wr index char count
				line = line.substr(3);
				index = stoi(line, &sz);
				line = line.substr(sz+1);
				Char = line.c_str()[0];
				line = line.substr(1);
				count = stoi(line, &sz);

				if (index < 1)
				{	throw "wr::write forbbiden";
				}
				count = count - FS->write(index, &Char, count);
				oFile << count << " bytes written" << endl;
			}//count bytes written
			else if (command.find("sk") == 0)
			{// sk index pos
				line = line.substr(3);
				index = stoi(line, &sz);
				line = line.substr(sz);
				pos = stoi(line, &sz);
				pos = FS->lseek(index, pos);
				oFile << "position is " << pos << endl;
			}//position is <pos>
			else if (command.find("dr") == 0)
			{// dr 
				out = FS->directory();
				oFile << out << endl;
			}//file1 file2 file3 ...
			else if (command.find("in") == 0)
			{// in disk_cont.txt
				if (line.length() > 7)
				{
					disk_cont = line.substr(3);
					out = FS->init(&disk_cont);
				}
				else
				{
					out = FS->init();
					disk_cont = "";
				}
				oFile << out << endl;
			}// disk initialized | disk restored
			else if (command.find("sv") == 0)
			{// sv disk_count.txt
				disk_cont = line.substr(3);
				FS->save(&disk_cont);
				oFile << "disk saved" << endl;
			}// disk saved
			else 
			{
				//throw exception("command error");
			}
		}
		catch (exception& s)
		{
			oFile << "error" << endl;
		}
	}
	iFile.close();
	oFile.close();
	cin.get();
	return -1;
}