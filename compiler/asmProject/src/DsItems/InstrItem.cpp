/*
 * InstrItem.cpp
 *
 *  Created on: Apr 2, 2024
 *      Author: John
 */

#include <set>
#include "OperandUtils.h"
#include "../Translate/TargetUtils.h"

#include "InstrItem.h"

using namespace std;


// TODO- instances created from tokens on action stack.
InstrItem::InstrItem(std::stack<AsmToken*> actStack) {
	// Initial values.
	m_opcode = TOKEN_INVALID;
	m_flagStr = "";
	m_regs = {};
	m_immediate = 0;
	m_flagMask = 0;

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

// TODO- analyze item for later checks and translation.
void InstrItem::doAnalysis(AnalysisData_t* model) {
	// No major analysis- immediates checked during type checks. //

	// Bookkeeping- update model.
	model->m_textLen += 2;        // 16-bit instruction
	model->m_lastAddrItem = this; // can be addressed/labeled
}

// TODO- type check item- return indicates "hasError".
bool InstrItem::doChecking(AnalysisData_t* model) {
	// Return code.
	bool hasErr = false;

	// Walk through flags- reporting repeats and no-effects.
	set<char> seenChar = {};
	for (int i = 1; i < (int)(m_flagStr.size()); i++) { // i = 1, skip '%'
		// Get next char/flag.
		char flag = m_flagStr[i];

		// Check/report if a repeat.
		if (seenChar.find(flag) != seenChar.end()) {
			cout << "WARN: (" << m_origin << "): Repeat flag (";
			cout << "flag = '" << flag << "')" << endl;
		}
		else { // Otherwise, record as seen.
			seenChar.insert(flag);
		}

		// Check if flag makes sense for instruction.
		if (!TargetUtils_instrUsesFlag(m_opcode, flag)) {
			cout << "WARN: (" << m_origin << "): No-effect flag (";
			cout << "flag = '" << flag << "')" << endl;
		}
	}

	// While we're at it- convert flags into bit-mask for translation.
	m_flagMask = TargetUtils_flagsToField(m_opcode, m_flagStr);


	// Re-size immediate value, reporting if truncation occurred.
	int newVal = TargetUtils_sizeToInstr(m_immediate, m_opcode);
	if (m_immediate != newVal) {
		cout << "WARN (" << m_origin << "): Immediate truncation (";
		cout << "oldVal=" << m_immediate << ", ";
		cout << "newVal=" << newVal << ")" << endl;

		// Save for translation.
		m_immediate = newVal;
	}

	// Return result of checking.
	return hasErr;
}

// TODO- resolve last of (meta) data before translating.
void InstrItem::resolveData(TranslationData_t* model) {
	// TODO- could resolve flag/immediate into parts here instead of type checking.

	// Claim next available text address (adjusting as needed).
	m_address = model->m_nextTextAddr;
	model->m_nextTextAddr += 2; // instruction takes 2 bytes
}

// TODO- get section item is in (most do, some don't).
SectionType_e InstrItem::getSection(void) {
	// Instructions always are in text section.
	return SECTION_TEXT;
}

// TODO- getters for translation.
SyntaxToken_e InstrItem::getOpcode(void) {return m_opcode;}
int InstrItem::getFlagMask(void) {return m_flagMask;}
int InstrItem::getRegIdx(int idx) {return (idx < (int)(m_regs.size())) ? m_regs[idx] : -1;}
int InstrItem::getImmediate(void) {return m_immediate;}

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
