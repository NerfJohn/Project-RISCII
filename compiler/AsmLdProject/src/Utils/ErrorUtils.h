/*
 * ErrorUtils.h
 *
 * "Helper functions to ease setting of errors/warnings in data model"
 */

#ifndef SRC_UTILS_ERRORUTILS_H_
#define SRC_UTILS_ERRORUTILS_H_

#include "DomainLayer/DataModel_t.h"

/*
 * Handles business logic of recording warnings/error in data model.
 *
 * Effectively a wrapper for localized business logic related to keeping track
 * of potentially build failing scenarios. Updates model accordingly.
 *
 * @param model data model of the program
 * @param reason exit reason that must be incorporated into the model
 */
void ErrorUtils_includeReason(DataModel_t* model, ExitReasonType_e reason);

#endif /* SRC_UTILS_ERRORUTILS_H_ */
