/*
 * main.cpp
 *
 * "Top-level of RISCII assembler/linker program"
 */

#include "AppLayer/CheckProgramStep.h"
#include "AppLayer/GenerateImageStep.h"
#include "AppLayer/ParseCliStep.h"
#include "AppLayer/ReadFilesStep.h"
#include "DeviceLayer/LabelTable.h"
#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"

using namespace std;

//==============================================================================
// The RISCII assembler/linker- Host PC runs entire program from here.
int main(int argc, char* argv[]) {
	// Initialize label table (for data model).
	LabelTable labelTable = LabelTable();

	// Initialize program's data model.
	DataModel_t prgmData = {
		// Values related to warning and error detection/reporting.
		.m_numWarnings  = 0,                     // no warnings at start
		.m_numErrors    = 0,                     // no errors at start
		.m_firstReason  = REASON_SUCCESS,        // innocent till guilty

		// Values related to parsed command line input.
		.m_helpFlag     = false,                 // don't print help menu
		.m_weFlag       = false,                 // warnings aren't errors
		.m_logLevel     = LOG_WARN,              // print errors/warnings
		.m_inFiles      = vector<string>(),      // no inputs files at start
		.m_outFile      = "",                    // no specific binary name

		// Values related to parsing/analyzing/checking the entire program.
		.m_openLabels   = vector<string>(),      // no open labels to start
		.m_labelTable   = labelTable,            // empty table
		.m_items        = vector<IBuildItem*>(), // no items at start
		.m_prgmStart    = nullptr,               // no initial program start
		.m_numTextBytes = 0,                     // no initial bytes
		.m_numDataBytes = 0,                     // no initial bytes
		.m_numBssBytes  = 0,                     // no initial bytes

		// Values related to prepping for file writing.
		.m_textSection  = vector<uint16_t>(),    // no initial section
		.m_dataSection  = vector<uint16_t>(),    // no initial section
		.m_bssSection   = 0                      // no initial bytes
	};

	// Handle the command line inputs before formal assembling.
	ParseCliStep_parseCli(argc, argv, prgmData);

	// Read in the input files, parsing them into the model.
	Printer::getInst()->log(LOG_INFO, "Reading inputs files...");
	ReadFilesStep_readFiles(&prgmData);

	// Analyze/check entire program before starting translation.
	Printer::getInst()->log(LOG_INFO, "Running program level checks...");
	CheckProgramStep_checkProgram(&prgmData);

	// Generate the binary image.
	Printer::getInst()->log(LOG_INFO, "Generating binary image...");
	GenerateImageStep_generateImage(prgmData);

	// Assembled successfully- return as such.
	Printer::getInst()->printSummary(prgmData); // log(LOG_INFO, ...)
	Printer::getInst()->printLog();             // empty out log prior to exit
	Terminate::getInst()->exit(REASON_SUCCESS);
}


