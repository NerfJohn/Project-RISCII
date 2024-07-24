/*
 * main.cpp
 *
 * "Top-level of RISCII assembler/linker program"
 */

#include "AppLayer/CheckProgramStep.h"
#include "AppLayer/GenerateImageStep.h"
#include "AppLayer/ParseCliStep.h"
#include "AppLayer/ReadFilesStep.h"
#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"

#include <iostream> // TODO- delete after development

using namespace std;

//==============================================================================
// The RISCII assembler/linker- Host PC runs entire program from here.
int main(int argc, char* argv[]) {
	// Initialize program's data model.
	DataModel_t prgmData = {
		// Values related to warning and error detection/reporting.
		.m_numWarnings = 0,                    // # of warnings found
		.m_numErrors   = 0,                    // # of errors found
		.m_firstReason = REASON_SUCCESS,       // reason for build failure

		// Values related to parsed command line input.
		.m_inFiles     = vector<string>(),     // passed in input files
		.m_outFile     = "",                   // filename to use for binary

		// Values related to parsing/analyzing/checking the entire program.
		.m_items       = vector<IBuildItem*>() // item-ized program
	};

	// Handle the command line inputs before formal assembling.
	Printer::getInst()->log(LOG_INFO, "Step 1/4- Parsing CLI Inputs");
	ParseCliStep_parseCli(argc, argv, &prgmData);

	// Read in the input files, parsing them into the model.
	Printer::getInst()->log(LOG_INFO, "Step 2/4- Reading Input Files");
	ReadFilesStep_readFiles(&prgmData);

	// Analyze/check entire program before starting translation.
	Printer::getInst()->log(LOG_INFO, "Step 3/4- Running Final Checks");
	CheckProgramStep_checkProgram(&prgmData);

	// Generate the binary image.
	Printer::getInst()->log(LOG_INFO, "Step 4/4- Generating Binary Image");
	GenerateImageStep_generateImage(prgmData);

	// Assembled successfully- return as such.
	Printer::getInst()->printSummary(prgmData);
	Printer::getInst()->printLog();
	Terminate::getInst()->exit(REASON_SUCCESS);
}


