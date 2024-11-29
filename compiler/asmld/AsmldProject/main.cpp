/*
 * main.cpp: Main elements/execution of the assembler/linker program.
 */

// TODO- clean up this file once the dust settles...

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "State/StepGenImage.h"
#include "State/StepParseCli.h"
#include "State/StepReadFiles.h"

using namespace std;

//==============================================================================
// Start/main process of the asmld.exe program.
int main(int argc, char* argv[]) {
	// Initialize program's shared data.
	DataModel_t prgmData;
	{
		// General Summary/Progress.
		prgmData.m_numWarns   = 0;
		prgmData.m_numErrs    = 0;               // no errors yet...
		prgmData.m_retCode    = RET_SUCCESS;     // INNOCENT till guilty

		// Parsed Cli Data.
		prgmData.m_files      = {};              // no initial files
		prgmData.m_outFile    = "";              // no initial out filename

		// Parsed/analyzed program.
		prgmData.m_nodes      = {};              // no initial nodes
		prgmData.m_openLabels = {};              // no initial unpaired labels

		// Binary image results.
		prgmData.m_textSize   = 0;               // no initial bytes
		prgmData.m_dataSize   = 0;               // no initial bytes
		prgmData.m_bssSize    = 0;               // no initial bytes
		prgmData.m_textVals   = {};              // no initial words
		prgmData.m_dataVals   = {};              // no initial words
	}

	// Log warnings/error by default. // TODO- "Debug" for developing
	Print::inst().setLogLevel(LOG_DEBUG);

	// Parse/Handle arguments and files into desired program.
	StepParseCli_execute(prgmData, argc, argv);
	StepReadFiles_execute(prgmData);
	// TODO- process program- global linking, debug, etc

	// Create the unified output program.
	StepGenImage_execute(prgmData);

	// Program finished- job well done.
	Terminate_summary(prgmData);
}
