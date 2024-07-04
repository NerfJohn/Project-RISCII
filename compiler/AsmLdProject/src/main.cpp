/*
 * main.cpp
 *
 * "Top-level of RISCII assembler/linker program"
 */

#include <iostream> // TODO- likely delete

#include "AppLayer/ParseCliStep.h"
#include "DeviceLayer/Terminate.h"

using namespace std;

//==============================================================================
// The RISCII assembler/linker- Host PC runs entire program from here.
int main(int argc, char* argv[]) {
	// Initialize program's data model.
	DataModel_t prgmData = {
		.m_inFiles = vector<string>()
	};

	// Handle the command line inputs before formal assembling.
	ParseCliStep_parseCli(argc, argv, &prgmData);

	// Assembled successfully- return as such.
	Terminate::getInst()->exit(REASON_SUCCESS);
}


