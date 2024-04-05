/*
 * InfoUtils.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: John
 */

#include "InfoUtils.h"

using namespace std;

// TODO- Gathers data on list of items (ie semantic analysis).
void InfoUtils_analyzeItems(std::vector<ABuildItem*> items, AnalysisData_t* model) {
	// New analysis- reset short-term variables.
	model->m_lastAddrItem = nullptr;

	// Iterate backwards- eases tying labels to items.
	for (int i = items.size() - 1; 0 <= i; i--) {
		// Have item add/modify it's info as needed.
		items[i]->doAnalysis(model);
	}
}

// TODO- Runs checks on given items/model (ie type checking).
void InfoUtils_typeCheckItems(std::vector<ABuildItem*> items, AnalysisData_t* model) {
	// TODO- implement
}
