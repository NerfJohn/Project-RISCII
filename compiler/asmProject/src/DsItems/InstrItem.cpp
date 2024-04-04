/*
 * InstrItem.cpp
 *
 *  Created on: Apr 2, 2024
 *      Author: John
 */

#include "OperandUtils.h"

#include "InstrItem.h"

using namespace std;


// TODO- instances created from tokens on action stack.
InstrItem::InstrItem(std::stack<AsmToken*> actStack) {
	// Initial values.
	m_opcode = TOKEN_INVALID;
	m_flagStr = "";
	m_regs = {};
	m_immediate = 0;

	// Pop/save immediate if present.
	if (actStack.top()->getType() == TOKEN_IMMEDIATE) {
		m_immediate = OperandUtils_immStrToInt(actStack.top()->getValue());
		actStack.pop();
	}

	// Pop/save registers.
	while(actStack.top()->getType() == TOKEN_REGISTER) {
		// Push to front (undoes stack's reverse order).
		m_regs.push_front(OperandUtils_regStrToInt(actStack.top()->getValue()));
		actStack.pop();
	}

	// Pop/save flags if present.
	if (actStack.top()->getType() == TOKEN_FLAG) {
		m_flagStr = actStack.top()->getValue();
		actStack.pop();
	}

	// Pop/save opcode (opcode defines origin of overall instruction).
	m_opcode = actStack.top()->getType();
	m_origin = actStack.top()->getOrigin();
	actStack.pop();
}

// TODO- "to string" for ease of debugging.
std::string InstrItem::asStr(void) {
	// Get prefix.
	string retStr = "{InstrItem: ";

	// Get contents of instance.
	retStr += "m_opcode(#) = " + to_string(m_opcode) + ", ";
	retStr += "m_flagStr = \"" + m_flagStr + "\", ";
	retStr += "m_regs = {";
	for (int i = 0; i < (int)m_regs.size(); i++) {
		retStr += to_string(m_regs[i]) + ", ";
	}
	retStr += "}, ";
	retStr += "m_immediate = " + to_string(m_immediate) + ", ";
	retStr += "m_origin = \"" + m_origin + "\"";

	// Get suffix.
	retStr += "}";

	// Return string-ified AsmToken.
	return retStr;
}

// Std. destructor.
InstrItem::~InstrItem(void) {/* no non-std pointers */}
