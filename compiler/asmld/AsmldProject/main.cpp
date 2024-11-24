/*
 * main.cpp: Main elements/execution of the assembler/linker program.
 */

// TODO- clean up this file once the dust settles...

#include "Device/Print.h"
#include "Device/Terminate.h"
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
		prgmData.m_numWarns = 0;
		prgmData.m_numErrs  = 0;                 // no errors yet...
		prgmData.m_retCode  = RET_SUCCESS;       // INNOCENT till guilty

		// Parsed Cli Data.
		prgmData.m_files    = {};                // no initial files
	}

	// Log warnings/error by default. // TODO- "Debug" for developing
	Print::inst().setLogLevel(LOG_DEBUG);

	// Core program: parse/handle input arguments and files.
	StepParseCli_execute(prgmData, argc, argv);
	StepReadFiles_execute(prgmData);

	// Program finished- job well done.
	Terminate_summary(prgmData);
}
