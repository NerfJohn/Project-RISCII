/*
 * main.cpp: Main elements/execution of the c-preprocessor program.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "State/StepParseCli.h"
#include "State/StepReadFiles.h"

using namespace std;

//==============================================================================
// Start/main process of the cprep.exe program.
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
	}

	// Log warnings/error by default.
	Print::inst().setLogLevel(LOG_WARNING);

	// Parse/Handle arguments and files into desired program.
	StepParseCli_execute(prgmData, argc, argv);

	// Process each input file individually.
	for (std::string fname : prgmData.m_files) {
		Print::inst().log(LOG_INFO, "='" + fname + "' Process=");
		StepReadFiles_execute(prgmData, fname);
	}

	// Program finished- job well done.
	Terminate_summary(prgmData);
}
