/*
 * InstructionItem.cpp
 *
 * "Item defining a binary instruction written in assembly"
 */

#include "Items/InstructionItem.h"

using namespace std;

//==============================================================================
//
InstructionItem::InstructionItem(std::queue<ScanToken_t*>* tokens) {
	// Begin with known values.
	m_opcode    = nullptr;
	m_flag      = nullptr;
	m_regs      = {};
	m_immediate = nullptr;

	// Filter each token type into its appropriate slot.
	while (!tokens->empty()) {
		// Filter the token...
		ScanToken_t* tkn = tokens->front();
		switch (tkn->m_lexTkn) {
		    case TOKEN_FLAGS:     m_flag = tkn; break;
		    case TOKEN_REGISTER:  m_regs.push_back(tkn); break;
		    case TOKEN_IMMEDIATE: m_immediate = tkn; break;
			default:              m_opcode = tkn; break;        // assume opcode
		}

		// And pop it from queue.
		tokens->pop();
	}
}

//==============================================================================
//
InstructionItem::~InstructionItem(void) {
	// Hard ownership of data- delete with the item.
	delete m_opcode;
	delete m_flag;
	for (ScanToken_t* tkn : m_regs) {delete tkn;}
	delete m_immediate;
}
