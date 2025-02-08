/*
 * Terminate.cpp: Standardizes program exit scenarios.
 */

#include "Common/Device/Print.h"
#include "Common/Os/OsExit.h"

#include "Common/Device/Terminate.h"

using namespace std;

//==============================================================================

// Local save of exit code settings.
int Terminate::s_assertCode = 1;     // avoid 0 as a common "success" code

//==============================================================================
// Retrieves the singleton instance for operations.
Terminate& Terminate::inst(void) {
	// Static instance- only one is ever created.
	static Terminate s_instance;

	// Return singleton.
	return s_instance;
}

//==============================================================================
// Sets assert exit code for assert().
void Terminate::setAssertCode(int const exitCode) {
	// Simple setter.
	s_assertCode = exitCode;
}

//==============================================================================
// Terminates program (without output besides given return code).
void Terminate::silent(int const exitCode) {
	// Behold- the Irish goodbye.
	OsExit_exit(exitCode);
}

//==============================================================================
// Prints given message as assert, then terminates.
void Terminate::assert(std::string const& msg) {
	// Print message explaining assert.
	Print::inst().bug(msg);

	// Next, immediately terminate as an assert.
	OsExit_exit(s_assertCode);
}

//==============================================================================
// Prints summary of program's status, then terminates.
void Terminate::summary(PrgmInfo_t const& model) {
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
