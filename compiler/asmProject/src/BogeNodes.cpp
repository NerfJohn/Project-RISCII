/*
 * BogeNodes.cpp
 *
 *  Created on: Feb 19, 2024
 *      Author: John
 */

#include <iostream>
#include "BogeNodes.h"

using namespace std;

// TODO
InstrItem::InstrItem(std::stack<AsmToken>* actStack) {
	// Init values.
	m_immStr = "---";
	m_regsStr = {};
	m_flagStr = "---";
	m_opStr = "---";
	m_opTkn = TOKEN_INVALID;

	// Check if immediate is present.
	if (actStack->top().getType() == TOKEN_IMMEDIATE) {
		m_immStr = actStack->top().getValue();
		actStack->pop();
	}

	// Grab registers as able.
	while (actStack->top().getType() == TOKEN_REGISTER) {
		m_regsStr.push_front(actStack->top().getValue()); // invert the order
		actStack->pop();
	}

	// Check if flags are present.
	if (actStack->top().getType() == TOKEN_FLAG) {
		m_flagStr = actStack->top().getValue();
		actStack->pop();
	}

	// Save opcode.
	m_opStr = actStack->top().getValue();
	m_opTkn = actStack->top().getType();
	actStack->pop();
}

// TODO
void InstrItem::print(void) {
	cout << "InstrItem: Tkn = " << m_opStr
		 << ", Flag = " << m_flagStr << ", # Regs = "
		 << m_regsStr.size() << ", imm = " << m_immStr
		 << endl;
}

// TODO
FuncItem::FuncItem(std::stack<AsmToken>* actStack) {
	// Init values.
	m_labelStr = "---";
	m_regsStr = {};
	m_funcStr = "---";
	m_funcTkn = TOKEN_INVALID;

	// Grab label.
	m_labelStr = actStack->top().getValue();
	actStack->pop();

	// Grab registers as able.
	while (actStack->top().getType() == TOKEN_REGISTER) {
		m_regsStr.push_front(actStack->top().getValue()); // invert the order
		actStack->pop();
	}

	// Save name/type info.
	m_funcStr = actStack->top().getValue();
	m_funcTkn = actStack->top().getType();
	actStack->pop();
}

// TODO
void FuncItem::print(void) {
	cout << "FuncItem: Tkn = " << m_funcStr
		 << ", # Regs = " << m_regsStr.size()
		 << ", label = " << m_labelStr
		 << endl;
}

// TODO
DataItem::DataItem(std::stack<AsmToken>* actStack) {
	// Init values.
	m_immStr = "---";
	m_str = "";
	m_dataStr = "---";
	m_dataTkn = TOKEN_INVALID;

	// Grab immediate value.
	m_immStr = actStack->top().getValue();
	actStack->pop();


	// Grab strings as able.
	while (actStack->top().getType() == TOKEN_STRING) {
		m_str += actStack->top().getValue(); // invert the order
		actStack->pop();
	}

	// Save name/type info.
	m_dataTkn = actStack->top().getType();
	if (m_dataTkn == TOKEN_WORD) {m_dataStr = ".word";}
	else {m_dataStr = ".arry";}
	actStack->pop();
}

// TODO
void DataItem::print(void) {
	cout << "DataItem: Tkn = " << m_dataStr
		 << ", String size = " << m_str.size()
		 << ", imm = " << m_immStr
		 << endl;
}

// TODO
DirectiveItem::DirectiveItem(std::stack<AsmToken>* actStack) {
	// Init values.
	m_labelStr = "---";
	m_directStr = "---";
	m_directTkn = TOKEN_INVALID;

	// Grab label.
	m_labelStr = actStack->top().getValue();
	actStack->pop();

	// Save name/type info.
	m_directStr = actStack->top().getValue();
	m_directTkn = actStack->top().getType();
	actStack->pop();
}

// TODO
void DirectiveItem::print(void) {
	cout << "DirectiveItem: Tkn = " << m_directStr
		 << ", label = " << m_labelStr
		 << endl;
}

// TODO
LabelItem::LabelItem(std::stack<AsmToken>* actStack) {
	// Init values.
	m_labelStr = "---";


	// Discard colon.
	actStack->pop();

	// Save name/type info.
	m_labelStr = actStack->top().getValue();
	actStack->pop();
}

// TODO
void LabelItem::print(void) {
	cout << "LabelItem: Label = " << m_labelStr
		 << endl;
}
