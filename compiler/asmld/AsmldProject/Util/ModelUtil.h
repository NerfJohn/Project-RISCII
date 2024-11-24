/*
 * ModelUtil.h: Utilities for updating/querying the program's data model.
 */

#ifndef UTIL_MODELUTIL_H_
#define UTIL_MODELUTIL_H_

#include "Domain/DataModel_t.h"
#include "Domain/RetCode_e.h"

/*
 * Update the model with the most recent error/resulting return code.
 *
 * Increments the count of found errors while saving the return code related
 * to the error for future use. The return code overwrites the previously
 * recorded one (ie the return code reflect the most recently found error).
 *
 * @param model   shared data of the entire program
 * @param errCode return code, related to the error, to save
 */
void ModelUtil_recordError(DataModel_t& model, RetCode_e const errCode);

/*
 * Update the model with the most recent warning.
 *
 * Increments the count of found warnings- allowing for reports the cumulative
 * number of warnings later on.
 *
 * @param model shared data of the entire program
 */
void ModelUtil_recordWarn(DataModel_t& model);

#endif /* UTIL_MODELUTIL_H_ */
