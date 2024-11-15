/*
 * main.cpp: Main elements/execution of the assembler/linker program.
 */

// TODO- clean up this file once the dust settles...

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "State/StepParseCli.h"

using namespace std;

//==============================================================================
// Start/main process of the asmld.exe program.
int main(int argc, char* argv[]) {
	// Initialize program's shared data.
	DataModel_t prgmData;
	{
		// Parsed Cli Data.
		prgmData.m_files = {};                   // no initial files
	}

	// Log warnings/error by default. // TODO- "Debug" for developing
	Print::inst().setLogLevel(LOG_DEBUG);

	// Parse/handle cli inputs into data model.
	StepParseCli_execute(prgmData, argc, argv);

	// Program finished- job well done. // TODO- exit w/ summary info
	Terminate_silent(RET_SUCCESS);
}
