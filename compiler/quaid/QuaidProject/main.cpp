/*
 * main.cpp: Head of the program/application.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Domain/DataModel_t.h"
#include "Domain/RetCode_e.h"
#include "State/StepParseCli.h"

using namespace std;

//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {
	// Init devices.
	Print::inst().setLogName("quaid.exe");
	Print::inst().setLogLevel(LOG_WARNING);
	Terminate::inst().setAssertCode(RET_ASSERT);

	// Init data model.
	DataModel_t prgmData = {
		// General Summary/Progress.
		.m_summary = {
			.m_numWarns = 0,
			.m_numErrs  = 0,
			.m_retCode  = RET_SUCCESS
		},

		// Parsed Cli Data.
		.m_files = {}
	};

	// Parse program's cli command/call.
	StepParseCli_execute(prgmData, argc, argv);

	// End program with summary of run instance.
	Terminate::inst().summary(prgmData.m_summary);
	return RET_SUCCESS; /* never reached, but appeal to g++ */
}
