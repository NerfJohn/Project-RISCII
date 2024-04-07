/*
 * LabelItem.cpp
 *
 *  Created on: Apr 3, 2024
 *      Author: John
 */

#include "LabelItem.h"

using namespace std;

// TODO- instances created from tokens on action stack.
LabelItem::LabelItem(std::stack<AsmToken*> actStack) {
	// Init values.
	m_name = "";

	// Pop off colon- simply to ease parsing.
	delete actStack.top();
	actStack.pop();

	// Save label name (also specifies origin).
	m_name = actStack.top()->getValue();
	m_origin = actStack.top()->getOrigin();
	delete actStack.top();
	actStack.pop();
}

// TODO- analyze item for later checks and translation.
void LabelItem::doAnalysis(AnalysisData_t* model) {
	// Label declared- mark it.
	model->m_table.addDecl(m_name, model->m_lastAddrItem);

	// For debugging- report details.
	cout << "INFO (" << m_origin << "): Label Decl ";
	cout << "(name =\"" << m_name << "\")" << endl;

	// No bookkeeping- no size, no address. //
}

// TODO- type check item- return indicates "hasError".
bool LabelItem::doChecking(AnalysisData_t* model) {
	// Return code.
	bool hasErr = false;

	// Get details of the label.
	LabelInfo_t info = model->m_table.getInfo(m_name);

	// Warn if label is never used.
	if (info.m_numUses == 0) {
		cout << "WARN: (" << m_origin << "): Unused label (";
		cout << "m_name=\"" << m_name << "\")" << endl;
	}

	// Report error if label goes nowhere.
	if (info.m_item == nullptr) {
		cout << "ERR: (" << m_origin << "): Label goes nowhere (";
		cout << "m_name=\"" << m_name << "\")" << endl;

		hasErr = true;
	}

	// Report results.
	return hasErr;
}

// TODO- resolve last of (meta) data before translating.
void LabelItem::resolveData(TranslationData_t* model) {
	// No translation data to resolve.
	return;
}

// TODO- get section item is in (most do, some don't).
SectionType_e LabelItem::getSection(void) {
	// Labels don't have sections themselves.
	return SECTION_INVALID;
}

// TODO- "to string" for ease of debugging.
std::string LabelItem::asStr(void) {
	// Get prefix.
	string retStr = "{LabelItem: ";

	// Get contents of instance.
	retStr += "m_name = \"" + m_name + "\", ";
	retStr += "m_origin = \"" + m_origin + "\"";

	// Get suffix.
	retStr += "}";

	// Return string-ified FuncItem.
	return retStr;
}

// Std. destructor.
LabelItem::~LabelItem(void) {/* no non-std pointers */}
