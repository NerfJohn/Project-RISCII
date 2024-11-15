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

#endif /* UTIL_MODELUTIL_H_ */
