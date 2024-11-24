/*
 * Terminate.cpp: Wrapper for handling different program exit scenarios.
 */

#include "Device/Print.h"
#include "Os/OsExit.h"

#include "Device/Terminate.h"

using namespace std;

//==============================================================================
// Terminates the program without any additional output/processing.
void Terminate_silent(RetCode_e const exitCode) {
	// Behold- the Irish goodbye.
	OsExit_exit(exitCode);

	/* Line never reached */
}

//==============================================================================
// Terminates using RET_ASSERT. Prints assert message just prior to exiting.
void Terminate_assert(std::string const& msg) {
	// Print message explaining assert.
	Print::inst().bug(msg);

	// Next, immediately terminate as an assert.
	OsExit_exit(RET_ASSERT);

	/* Line never reached */
}

//==============================================================================
// Terminates using model data- logging a summary of program's results.
void Terminate_summary(DataModel_t const& model) {
	// Log a summary of the program's efforts.
	string sumStr = to_string(model.m_numErrs)  +
			        " errors, "                 +
					to_string(model.m_numWarns) +
					" warnings, returned "      +
					to_string((int)(model.m_retCode));
	Print::inst().log(LOG_INFO, sumStr);

	// Exit with stated return code.
	OsExit_exit(model.m_retCode);

	/* Line never reached */
}
