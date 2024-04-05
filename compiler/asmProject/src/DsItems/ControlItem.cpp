/*
 * ControlItem.cpp
 *
 *  Created on: Apr 3, 2024
 *      Author: John
 */

#include "ControlItem.h"

using namespace std;

// TODO- instances created from tokens on action stack.
ControlItem::ControlItem(std::stack<AsmToken*> actStack) {
	// Init. values.
	m_type = TOKEN_INVALID;
	m_label = "";

	// Save label.
	m_label = actStack.top()->getValue();
	delete actStack.top();
	actStack.pop();

	// Save type of directive (acts as origin of item).
	m_type = actStack.top()->getType();
	m_origin = actStack.top()->getOrigin();
	delete actStack.top();
	actStack.pop();
}

// TODO- analyze item for later checks and translation.
void ControlItem::doAnalysis(AnalysisData_t* model) {
	// Record label associated with control.
	if (m_type == TOKEN_GLBL) {model->m_glblLabel = m_label;}
	else {model->m_heapLabel = m_label;}

	// Technically, a use of the label.
	model->m_table.addUse(m_label);

	// For debugging- report details.
	cout << "INFO (" << m_origin << "): Label Use ";
	cout << "(name =\"" << m_label << "\")" << endl;

	// No bookkeeping- no final size and non-addressable. //
}

// TODO- "to string" for ease of debugging.
std::string ControlItem::asStr(void) {
	// Get prefix.
	string retStr = "{ControlItem: ";

	// Get contents of instance.
	retStr += "m_type(#) =" + to_string(m_type) + ", ";
	retStr += "m_label = \"" + m_label + "\", ";
	retStr += "m_origin = \"" + m_origin + "\"";

	// Get suffix.
	retStr += "}";

	// Return string-ified FuncItem.
	return retStr;
}

// Std. destructor.
ControlItem::~ControlItem(void) {/* no non-std pointers */}
