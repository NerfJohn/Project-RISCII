/*
 * FuncItem.cpp
 *
 *  Created on: Apr 3, 2024
 *      Author: John
 */

#include "OperandUtils.h"

#include "FuncItem.h"

using namespace std;

// TODO- instances created from tokens on action stack.
FuncItem::FuncItem(std::stack<AsmToken*> actStack) {
	// Init values.
	m_reg = 0;
	m_label = "";

	// Save label.
	m_label = actStack.top()->getValue();
	delete actStack.top();
	actStack.pop();

	// Save register.
	m_reg = OperandUtils_regStrToInt(actStack.top()->getValue());
	delete actStack.top();
	actStack.pop();

	// Use function for origin source.
	m_origin = actStack.top()->getOrigin();
	delete actStack.top();
	actStack.pop();
}

// TODO- analyze item for later checks and translation.
void FuncItem::doAnalysis(AnalysisData_t* model) {
	// _la uses label- record usage.
	model->m_table.addUse(m_label);

	// For debugging- report details.
	cout << "INFO (" << m_origin << "): Label Use ";
	cout << "(name =\"" << m_label << "\")" << endl;

	// Bookkeep.
	model->m_textLen += 4;        // _la = 2 16-bit instructions
	model->m_lastAddrItem = this; // can be addressed/labeled
}

// TODO- "to string" for ease of debugging.
std::string FuncItem::asStr(void) {
	// Get prefix.
	string retStr = "{FuncItem: ";

	// Get contents of instance.
	retStr += "note: only _la, ";
	retStr += "m_reg = " + to_string(m_reg) + ", ";
	retStr += "m_label = \"" + m_label + "\", ";
	retStr += "m_origin = \"" + m_origin + "\"";

	// Get suffix.
	retStr += "}";

	// Return string-ified FuncItem.
	return retStr;
}

// Std. destructor.
FuncItem::~FuncItem(void) {/* no non-std pointers */}
