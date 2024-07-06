/*
 * ErrorUtils.cpp
 *
 * "Helper functions to ease setting of errors/warnings in data model"
 */

#include "Utils/ErrorUtils.h"

//==============================================================================
// Handles business logic of recording warnings/error in data model.
void ErrorUtils_includeReason(DataModel_t* model, ExitReasonType_e reason) {
	// Handle reason based on which grouping it is in.
	if ((REASON_WARNING_MIN <= reason) && (reason <= REASON_WARNING_MAX)) {
		// A warning- track count, but not a reason to fail building.
		model->m_numWarnings++;
	}
	else if ((REASON_ERROR_MIN <= reason) && (reason <= REASON_ERROR_MAX)) {
		// An error- track count and, if the first error, record reason.
		model->m_numErrors++;
		if (model->m_firstReason == REASON_SUCCESS) { // ie not a failure
			model->m_firstReason = reason;
		}
	}

	/* Otherwise- given reason has no effect on error handling */
}

