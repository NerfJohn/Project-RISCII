/*
 * main.cpp: Head of the program/application.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Domain/DataModel_t.h"
#include "Domain/RetCode_e.h"
#include "Ds/IAstNode.h"
#include "State/StepParseCli.h"
#include "State/StepReadFile.h"

using namespace std;

//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {
	// Init devices.
	Print::inst().setLogName("ccomp.exe");
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
		.m_doOpt = false,                        // default- no optimization

		// Processed Data.
		.m_ast   = nullptr                       // AST of parsed file
	};

	// Parse program's cli command/call.
	StepParseCli_execute(prgmData, argc, argv);

	// Translate each file.
	Print::inst().log(LOG_INFO, "=Core Process=");
	for (string file : prgmData.m_files) {
		// Translate file.
		Print::inst().log(LOG_INFO, string("Translating '") + file + "'...");
		StepReadFiles_execute(prgmData, file);

		// Clean-up model (for next file).
		if (prgmData.m_ast != nullptr) {delete prgmData.m_ast;}
	}


	// End program with summary of run instance.
	Terminate::inst().summary(prgmData.m_summary);
	return RET_SUCCESS; /* never reached, but appeal to g++ */
}
