#include "loader.h"

using namespace std;

void Loadelf::Load_file(string filename)
{
	tmp = new char[64];
	ProH = new ProgramHead();
	ElfH = new ElfHead();
	SegH = new SegmentHead();

	file.open(filename.data(), ios::in|ios::binary);
	if(!file)
		file.close();

	file.read((char *)ElfH, 64);
	file.seekg((int)ElfH->getPhos(), ios::beg);
	file.read((char *)ProH, (int)ElfH->getPhsiz());

	file.seekg((int)ElfH->getShos() + 64*10, ios::beg);
	file.read((char *)SegH, (int)ElfH->getShsiz());

	Mem->setsize((int)ProH->getMs());
	file.seekg((int)ProH->getOs(), ios::beg);
	file.read((*(char**)Mem + 0x10000), (int)ProH->getFs());
	Mem->initial((int)SegH->getAddr(), (int)SegH->getSize());

	// file.seekg((int)0x4208, ios::beg);
	// file.read((char *)tmp, 64);
	// for(int i = 0; i < 64; i++)
	// {
	// 	if (tmp[i] >= 32 && tmp[i] <= 126)
 //  			cout << tmp[i] << " ";
 //        else
 //          cout << (int)(unsigned char)tmp[i] << " ";
	// }

	Mem->MyPrint();
	file.close();

	delete SegH;
	delete ElfH;
	delete ProH;
	delete tmp;
}

// int main()
// {
// 	loadelf *sysload = new loadelf();

// 	sysload->Load_file();

// 	delete sysload;
// }