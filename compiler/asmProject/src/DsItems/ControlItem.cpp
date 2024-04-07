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
	// Record item associated with control.
	if (m_type == TOKEN_GLBL) {model->m_glblLabel = this;}
	else {model->m_heapLabel = this;}

	// Technically, a use of the label.
	model->m_table.addUse(m_label);

	// For debugging- report details.
	cout << "INFO (" << m_origin << "): Label Use ";
	cout << "(name =\"" << m_label << "\")" << endl;

	// No bookkeeping- no final size and non-addressable. //
}

// TODO- type check item- return indicates "hasError".
bool ControlItem::doChecking(AnalysisData_t* model) {
	// Return code.
	bool hasErr = false;

	// Check control word is unique (ie should match model's records).
	ControlItem* control = (m_type == TOKEN_GLBL) ? model->m_glblLabel : model->m_heapLabel;
	if (control != this) {
		// Note: based on semantic analysis, m_item should NOT be null.
		cout << "ERR: (" << m_origin << "): Multiple controls (";
		cout << "this = " << this->asStr() << ", ";
		cout << "record = " << control->asStr() << ")" << endl;
		hasErr = true;
	}

	// Check label given is declared.
	if (model->m_table.getInfo(m_label).m_item == nullptr) {
		cout << "ERR (" << m_origin << "): Undeclared label (";
		cout << "m_label=\"" << m_label << "\")" << endl;
		hasErr = true;
	}
	else {
		// Label exists? then check item matches the expected section.
		SectionType_e sectExp = (m_type == TOKEN_GLBL) ? SECTION_TEXT : SECTION_BSS;
		SectionType_e sectAct = model->m_table.getInfo(m_label).m_item->getSection();
		if (sectExp != sectAct) {
			cout << "ERR (" << m_origin << "): Bad control label (";
			cout << "expected(#)=" << sectExp << ", ";
			cout << "actual(#)=" << sectAct << ")" << endl;
			hasErr = true;
		}
	}

	// Report results.
	return hasErr;
}

// TODO- resolve last of (meta) data before translating.
void ControlItem::resolveData(TranslationData_t* model) {
	// No translation data to resolve.
	return;
}

// TODO- get section item is in (most do, some don't).
SectionType_e ControlItem::getSection(void) {
	// Controls don't translate to a section.
	return SECTION_INVALID;
}

// TODO- get special label (unique to control items).
std::string ControlItem::getLabel(void) {
	return m_label;
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
