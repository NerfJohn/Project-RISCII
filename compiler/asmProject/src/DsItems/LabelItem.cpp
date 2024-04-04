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
