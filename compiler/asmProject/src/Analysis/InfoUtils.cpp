/*
 * InfoUtils.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: John
 */

#include <iostream>
#include "TargetUtils.h"
#include "ControlItem.h"

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
	// Err flag- report all warnings/errors, but prevent translation.
	bool hasErr = false;

	// Check translated program will fit onto target.
	int binSize = model->m_textLen + model->m_dataLen;
	int gblSize = model->m_dataLen + model->m_bssLen;
	if (binSize > TARGET_ROM_MAX) {
		cout << "ERR: binary too big (";
		cout << "max = " << TARGET_ROM_MAX << ", ";
		cout << "binary = " << binSize << ")" << endl;
		hasErr = true;
	}
	if (gblSize > TARGET_RAM_MAX) {
		cout << "ERR: data too big (";
		cout << "max = " << TARGET_RAM_MAX << ", ";
		cout << "binary = " << gblSize << ")" << endl;
		hasErr = true;
	}

	// Check program has been given a starting point.
	string startLbl = model->m_glblLabel->getLabel();
	if (model->m_table.getInfo(startLbl).m_item == nullptr) {
		cout << "ERR: .glbl not used/declared" << endl;
		hasErr = true;
	}

	// Type check program (in line order per file).
	for (int i = 0; i < (int)(items.size()); i++) {
		// Check item- tracking if errors have been reported.
		hasErr |= items[i]->doChecking(model);
	}

	// Exit for any found errors.
	if (hasErr) {exit(1);}
}
