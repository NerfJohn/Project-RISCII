/*
 * main.cpp: Head of the program/application.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Domain/DataModel_t.h"
#include "Domain/RetCode_e.h"
#include "State/StepParseCli.h"
#include "State/StepReadFiles.h"

using namespace std;

//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {
	// Init devices.
	Print::inst().setLogName("cprep.exe");
	Print::inst().setLogLevel(LOG_WARNING);
	Terminate::inst().setAssertCode(RET_ASSERT);

	// Init data model.
	PrgmInfo_t runSummary = {
		.m_numWarns = 0,                         // no initial warnings
		.m_numErrs  = 0,                         // no initial errors
		.m_retCode  = RET_SUCCESS                // good so far
	};
	DataModel_t prgmData = {
		// General Summary/Progress.
		.m_summary = runSummary,                 // (see above)

		// Parsed Cli Data.
		.m_files = {},                           // no initial files
		.m_iDirs = {},                           // no initial directories

		// Processed data.
		.m_srcAst   = nullptr,                   // begin without source
		.m_iAsts    = {},                        // include file ASTs
		.m_incPaths = {}                         // include file to parse
	};
	prgmData.m_iAsts.scopePush();

	// Parse program's cli command/call.
	StepParseCli_execute(prgmData, argc, argv);

	// (Pre)process each file.
	Print::inst().log(LOG_INFO, "=Core Process=");
	for (string file : prgmData.m_files) {
		StepReadFiles_execute(prgmData, file);
	}

	// End program with summary of run instance.
	Terminate::inst().summary(prgmData.m_summary);
	return RET_SUCCESS; /* never reached, but appeal to g++ */
}
