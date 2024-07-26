/*
 * ErrorUtils.cpp
 *
 * "Helper functions to ease setting of errors/warnings in data model"
 */

#include "Utils/ErrorUtils.h"

//==============================================================================
// Handles business logic of recording warnings/error in data model.
void ErrorUtils_includeReason(DataModel_t* model, ExitReasonType_e reason) {
	// Gather details on reason.
	bool isWarn = (REASON_WARNING_MIN <= reason) &&
			      (reason <= REASON_WARNING_MAX);
	bool isErr  = (REASON_ERROR_MIN <= reason) && (reason <= REASON_ERROR_MAX);

	// Increment correct counters.
	if (isWarn) {model->m_numWarnings++;}
	else if (isErr) {model->m_numErrors++;}

	// Record reason if it is the first fatal reason.
	if ((isErr || (isWarn && model->m_weFlag)) &&
	    (model->m_firstReason == REASON_SUCCESS)) {
		model->m_firstReason = reason;
	}

	/* Otherwise- given reason has no effect on error handling */
}

