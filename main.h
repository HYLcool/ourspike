// main.h
// define the classes need when we execute a program
// including RegisterFile, Memory, Instruction and so on


#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>

#include "machine.h"

using namespace std;

// some functions
void ShowUsage(); // print the usage message
void InitializeSystem(bool, bool); // initialize the data structures
void Clean(); // clean after finishing the program

// data structures
Machine * machine; // a machine to run the code