// main.cpp
// Main program
// the main program of this simulator
// a main routine for running a program

#include "main.h"

using namespace std;

// main routine
int main(int argc, char ** argv) {

	// if there is no arguments, print the usage message and return
	if (argc <= 1) {
		ShowUsage();
		return 0;
	}

	bool debug = false;
	bool instructionCount = false;
	bool trace = false;

	for (int i = 1; i < argc - 1; i++) {
		if (!strcmp(*(argv + i), "-d")) { // debugging mod
			debug = true;
		} else if (! strcmp(*(argv + i), "-u")) { // print the usage message
			ShowUsage();
			exit(0);
		} else if (! strcmp(*(argv + i), "-ic")) { // print the instruction count message
			instructionCount = true;
		} else if (! strcmp(*(argv + i), "-trace")) { // print the instruction trace of program
			trace = true;
		} else { // unknown argument
			printf("Unknown argument : %s\n", *(argv + i));
			exit(1);
		}
	}

	// get the executable file
	char * exeFilename = *(argv + (argc - 1));
	fstream exeFile;
	exeFile.open(exeFilename, ios::in);
	if (!exeFile) {
		printf("The file <%s> is not existed\n", exeFilename);
		exit(1);
	}
	exeFile.close();

	InitializeSystem(instructionCount, debug, trace);

	// Parse the elf file
	machine -> setFileName(string(exeFilename));

	// start executing the program
	machine -> Run();

	// something else ...
	Clean();

	return 0;
}

void ShowUsage() {
	printf("Our Simulator based on RISC-V 64 by Huang Yilun, Ye Gen, Xing Xinyu.\n\n");
	printf("Usage:\n\t./ourspike [-options] <filename>\n");
	printf("options: \n");
	printf("\t-u\tprint this usage message.\n");
	printf("\t-d\tfor debugging. In debug mode, you can click a '?' to get the debug help message.\n");
	printf("\t-ic\tprint the instruction count of the execution process\n");
	printf("\t-trace\tprint the executing trace of a program, cover the -d option\n");

	printf("\n");
}

void InitializeSystem(bool ic, bool debug, bool trace) {
	machine = new Machine(ic, debug, trace);
}

void Clean() {
	delete machine;
}