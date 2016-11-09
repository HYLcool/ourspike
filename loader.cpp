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
	file.read(*(char**)Mem, (int)ProH->getFs());
	Mem->initial((int)SegH->getAddr(), (int)SegH->getSize());

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