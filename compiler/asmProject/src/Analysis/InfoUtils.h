/*
 * InfoUtils.h
 *
 *  Created on: Apr 4, 2024
 *      Author: John
 */

#ifndef SRC_ANALYSIS_INFOUTILS_H_
#define SRC_ANALYSIS_INFOUTILS_H_

#include <vector>
#include "ABuildItem.h"
#include "AnalysisData_t.h"

// TODO- Gathers data on list of items (ie semantic analysis).
void InfoUtils_analyzeItems(std::vector<ABuildItem*> items, AnalysisData_t* model);

// TODO- Runs checks on given items/model (ie type checking).
void InfoUtils_typeCheckItems(std::vector<ABuildItem*> items, AnalysisData_t* model);

#endif /* SRC_ANALYSIS_INFOUTILS_H_ */
