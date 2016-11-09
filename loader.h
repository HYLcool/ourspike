#include <iostream>
#include <fstream>
#include <string>

using namespace std;

typedef long long int lli;
typedef short int si;

#define VIRMEM_OFFSET 65536
#define MAX_SIZE 80000

class ElfHead{
  public:
	ElfHead(){};
	~ElfHead(){};

	lli getPhos(){return phoffset;};
	lli getShos(){return shoffset;};
	lli getPhsiz(){return ph_siz;};
	lli getShsiz(){return sh_siz;};
  lli getEntryAddr() {return entryaddr;}

  private:
	char Magic[16];

	lli other_info;

	lli entryaddr;
	lli phoffset;
	lli shoffset;

	int flags;

	si eh_siz;

	si ph_siz;
	si ph_cnt;

	si sh_siz;
	si sh_cnt;
	si str_index; 

};

class ProgramHead{
  public:
	ProgramHead(){};
	~ProgramHead(){};

	lli getOs(){return offset;};
	lli getFs(){return FileSiz;};
	lli getMs(){return MemSiz;};

  private:
  	lli type;
  	lli offset;
  	lli Virtaddr;
  	lli Physaddr;
  	lli FileSiz;
  	lli MemSiz;
  	lli Flag_Align;
};

class SegmentHead{
  public:
  	SegmentHead(){};
  	~SegmentHead(){};

  	lli getAddr(){return Address;};
  	lli getSize(){return size;};

  private:
  	int index;
  	int type;
  	lli flag;
  	lli Address;
  	lli fioffset;
  	lli size;

  	lli Link_info;
  	lli align;
  	lli entsize;
};



//A data instruction to simulate main memory.
class Memory{
  public:
  	Memory(){Mem = new char[MAX_SIZE];};
  	~Memory(){ delete Mem; };

  	void initial(int beginAd, int size){};
  	void readMem(int situation, char* p, int size)
  	{
  	  for(int i = 0; i < size; i++)
      {
        *(p + i) = Mem[situation - VIRMEM_OFFSET + i];
      }	
  	};
  	void writeMem(int situation, char* p, int size)
  	{
      for(int i = 0; i < size; i++)
      {
        Mem[situation - VIRMEM_OFFSET + i] = *(p + i);
      } 
  	}
  	void MyPrint()
  	{
  		for(int i = 0; i < 8; i++)
  		{
  			cout << (int)(unsigned char)Mem[i] << " ";
  		}
  	}
    void setsize(int s){Memsiz = s;};

  private:
  	char *Mem;
  	int Memsiz;
};

class Loadelf{
  public:
  Loadelf(Memory * mem){
    Mem = mem;
  };
  ~Loadelf(){};

  void Load_file(string);

  private:
    ifstream file;
    char *tmp;
    ElfHead *ElfH;
    ProgramHead *ProH;
    SegmentHead *SegH;
    Memory *Mem;
};
