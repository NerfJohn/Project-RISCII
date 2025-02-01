/*
 * Terminate.cpp: Standardizes program exit scenarios.
 */

#include "Common/Device/Print.h"
#include "Common/Os/OsExit.h"

#include "Common/Device/Terminate.h"

using namespace std;

//==============================================================================
// Terminates program (without output besides given return code).
void Terminate_silent(RetCode_e const exitCode) {
	// Behold- the Irish goodbye.
	OsExit_exit(exitCode);
}

//==============================================================================
// Prints given message as assert, then terminates with RET_ASSERT.
void Terminate_assert(std::string const& msg) {
	// Print message explaining assert.
	Print::inst().bug(msg);

	// Next, immediately terminate as an assert.
	OsExit_exit(RET_ASSERT);
}

//==============================================================================
// Prints summary of program's status, then terminates.
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
}
