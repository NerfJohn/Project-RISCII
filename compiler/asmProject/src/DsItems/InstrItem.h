/*
 * InstrItem.h
 *
 *  Created on: Apr 2, 2024
 *      Author: John
 */

#ifndef SRC_DSITEMS_INSTRITEM_H_
#define SRC_DSITEMS_INSTRITEM_H_

#include <stack>
#include <deque>
#include "ABuildItem.h"
#include "../Parsing/AsmToken.h"

// TODO- item representing instruction line item.
class InstrItem: public ABuildItem {
public:
	// TODO- instances created from tokens on action stack.
	InstrItem(std::stack<AsmToken*> actStack);

	// TODO- analyze item for later checks and translation.
	void doAnalysis(AnalysisData_t* model);

	// TODO- "to string" for ease of debugging.
	std::string asStr(void);

	// Std. destructor.
	~InstrItem(void);

private:
	// Typical instruction information.
	SyntaxToken_e    m_opcode;    // (limited to opcode tokens)
	std::string      m_flagStr;   // flags (preserved as string)
	std::deque<int>  m_regs;      // registers (converted to ints)
	int              m_immediate; // immediate (converted to int)
};

#endif /* SRC_DSITEMS_INSTRITEM_H_ */
