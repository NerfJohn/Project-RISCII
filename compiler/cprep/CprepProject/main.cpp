/*
 * main.cpp: Head of the program/application.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Domain/DataModel_t.h"
#include "Domain/RetCode_e.h"
#include "State/StepParseCli.h"

// TODO- delete after proper "read" state is made.
#include "Common/Device/File.h"
#include "State/SubStepLexFile.h"
#include "State/SubStepParseTkns.h"

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
		.m_files = {}                            // no initial files
	};

	// Parse program's cli command/call.
	StepParseCli_execute(prgmData, argc, argv);

	// TODO- delete after proper "read" state is made.
	File::inst().open(prgmData.m_files[0], FILE_OP_READ);
	queue<LexToken*> tkns;
	SubStepLexFile_execute(prgmData, tkns);
	SubStepParseTkns_execute(prgmData, tkns);

	// End program with summary of run instance.
	Terminate::inst().summary(prgmData.m_summary);
	return RET_SUCCESS; /* never reached, but appeal to g++ */
}
