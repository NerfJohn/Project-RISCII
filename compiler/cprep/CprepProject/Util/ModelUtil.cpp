/*
 * ModelUtil.cpp: Utilities for updating/querying the program's data model.
 */

#include "Util/ModelUtil.h"

using namespace std;

//==============================================================================
// Update the model with the most recent error/resulting return code.
void ModelUtil_recordError(DataModel_t& model, RetCode_e const errCode) {
	// Update model with newest error+code.
	model.m_numErrs++;
	model.m_retCode = errCode;
}

//==============================================================================
// Update the model with the most recent warning.
void ModelUtil_recordWarn(DataModel_t& model) {
	// Update model for found warning.
	model.m_numWarns++;
}
