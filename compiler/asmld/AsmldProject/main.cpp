/*
 * main.cpp: Main elements/execution of the assembler/linker program.
 */

// TODO- clean up this file once the dust settles...

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "State/StepParseCli.h"

using namespace std;

//==============================================================================
// TODO- define once main is ready...
int main(int argc, char* argv[]) {
	// Initialize program's shared data.
	DataModel_t prgmData;
	{
		// Parsed Cli Data.
		prgmData.m_files = {};                   // no initial files
	}

	// TODO- for now, print everything.
	Print::inst().setLogLevel(LOG_DEBUG);

	// First, parse in cli file/flag inputs.
	StepParseCli_execute(prgmData, argc, argv);

	// Return for a job well done. // TODO- likely return w/ summary in future
	Terminate_silent(RET_SUCCESS);
}
