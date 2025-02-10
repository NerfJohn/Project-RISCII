/*
 * InfoUtil.cpp: Utilities for updating/querying the program's data model.
 */

#include "Common/Util/InfoUtil.h"

using namespace std;

//==============================================================================
// Update progrm info/summary with most recent error/exit code.
void InfoUtil_recordError(PrgmInfo_t& summary, int const errCode) {
	// Update info with newest error+code.
	summary.m_numErrs++;
	summary.m_retCode = errCode;
}

//==============================================================================
// Update program info/summary with most recent warning.
void InfoUtil_recordWarn(PrgmInfo_t& summary) {
	// Update info for found warning.
	summary.m_numWarns++;
}
