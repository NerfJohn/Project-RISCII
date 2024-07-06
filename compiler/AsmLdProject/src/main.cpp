/*
 * main.cpp
 *
 * "Top-level of RISCII assembler/linker program"
 */

#include <iostream> // TODO- likely delete
#include "Utils/PrintUtils.h"
#include "DeviceLayer/FileReader.h"
#include "DeviceLayer/Printer.h"

#include "AppLayer/ParseCliStep.h"
#include "AppLayer/ReadFilesStep.h"
#include "DeviceLayer/Terminate.h"

using namespace std;

//==============================================================================
// The RISCII assembler/linker- Host PC runs entire program from here.
int main(int argc, char* argv[]) {
	// Initialize program's data model.
	DataModel_t prgmData = {
		// Values related to warning and error detection/reporting.
		.m_numWarnings = 0,               // # of warnings found
		.m_numErrors   = 0,               // # of errors found
		.m_firstReason = REASON_SUCCESS,  // reason for build failure

		// Values related to parsed command line input.
		.m_inFiles     = vector<string>() // passed in input files
	};

	// Handle the command line inputs before formal assembling.
	ParseCliStep_parseCli(argc, argv, &prgmData);

	// Read in the input files, parsing them into the model.
	ReadFilesStep_readFiles(&prgmData);

	// Assembled successfully- return as such.
	Terminate::getInst()->exit(REASON_SUCCESS);
}


