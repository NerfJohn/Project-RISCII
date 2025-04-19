/*
 * main.cpp: Head of the program/application.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Domain/DataModel_t.h"
#include "State/StepParseCli.h"
#include "Util/AppUtil.h"

using namespace std;

//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {
	// Init devices.
	Terminate_setBugCode(RET_ASSERT);
	Print_setLogLevel(LOG_WARNING);
	Print_setLogName("ccomp.exe");    // Set to name of program

	// Init program's data.
	DataModel_t model = {
		// General Summary/Progress.
		.m_numErrs  = 0,
		.m_numWarns = 0,
		.m_exitCode = RET_SUCCESS                // start optimistic
	};

	// Collect inputs.
	StepParseCli_execute(model, argc, argv);

	// Exit for successful run of program.
	AppUtil_exitSummary(model);
	return RET_SUCCESS; // Not reached- appeasing g++
}
