/*
 * AppUtil.cpp: Utilities specific to application level operations/data.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/Msg.h"

#include "Util/AppUtil.h"

using namespace std;

//==============================================================================
// Updates data model for occurrence of an error.
void AppUtil_error(DataModel_t& model, RetCode_e const code) {
	// Update- error affects exit code.
	model.m_numErrs++;
	model.m_exitCode = code;
}

//==============================================================================
// Updates data model for occurrence of a warning.
void AppUtil_warn(DataModel_t& model) {
	// Update.
	model.m_numWarns++;
}

//==============================================================================
// Prints summary of program's results, then exits.
void AppUtil_exitSummary(DataModel_t& model) {
	// Summarize progess.
	string inf = Msg() + model.m_numErrs        +
			             " errors, "            +
						 model.m_numWarns       +
						 " warnings, returned " +
						 model.m_exitCode;
	Print_log(LOG_INFO, inf);

	// Exit (with indicated code).
	Terminate_silent(model.m_exitCode);
}

//==============================================================================
// Prints "bug" message, then exits.
void AppUtil_exitBug(std::string const& msg) {
	// Assert bug.
	Print_bug(msg);
	Terminate_bug();
}
