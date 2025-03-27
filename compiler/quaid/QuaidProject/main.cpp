/*
 * main.cpp: Head of the program/application.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Domain/DataModel_t.h"
#include "Domain/RetCode_e.h"
#include "State/StepParseCli.h"

// TODO
#include "Device/Cmd.h"

using namespace std;

//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {
	// Init devices.
	Print::inst().setLogName("quaid.exe");
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
		.m_cFiles  = {},                         // no initial input files
		.m_sFiles  = {},                         // no initial input  files
		.m_depth   = PROC_TO_BIN,                // by default- create image
		.m_iDefs   = {},                         // no initial definitions
		.m_iDirs   = {},                         // no initial directories
		.m_doDbg   = false,                      // by default- don't debug
		.m_outFile = "",                         // no initial output name
		.m_optLvl  = OPT_LVL_NONE                // by default- don't optimize
	};

	// Parse program's cli command/call.
	StepParseCli_execute(prgmData, argc, argv);

	// TODO
	int a;
	prgmData.m_cFiles[0] += " -t";
	Print::inst().cli(to_string(Cmd_run(prgmData.m_cFiles[0])));


	// End program with summary of run instance.
	Terminate::inst().summary(prgmData.m_summary);
	return RET_SUCCESS; /* never reached, but appeal to g++ */
}
