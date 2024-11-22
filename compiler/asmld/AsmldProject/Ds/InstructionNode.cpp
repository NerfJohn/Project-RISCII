/*
 * InstructionNode.cpp: Represents single ISA instruction in assembly.
 */

#include "Device/File.h"
#include "Device/Print.h"
#include "Device/Terminate.h"

#include "Ds/InstructionNode.h"

using namespace std;

//==============================================================================
// Constructor called by parser. Builds itself directly from action stack.
InstructionNode::InstructionNode(BuildStack_t& itemStack) {
	// Parse all items from the stack (ie trusting parser knows its grammar).
	while (itemStack.size() > 0) {
		// (Sanity check item.)
		if (itemStack.top() == nullptr) {
			Terminate_assert("Null token while creating InstrNode");
		}

		// Move items based on type (ie REALLY trusting parser...).
		switch (itemStack.top()->m_lexTkn) {
			case TOKEN_KW_SHR:
				m_itemOp = move(itemStack.top());
				break;
			case TOKEN_FLAGS:
				m_itemFlag = move(itemStack.top());
				break;
			case TOKEN_REGISTER:
				m_itemRegs.push_back(move(itemStack.top()));
				break;
			case TOKEN_IMMEDIATE:
				m_itemImm = move(itemStack.top());
				break;
			default:
				// Non-matching token? compiler bug.
				Terminate_assert("Unknown token while creating InstrNode");
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// (Sanity check pertinent details.)
	if (m_itemOp == nullptr) {Terminate_assert("InstrNode has no opcode");}

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
					  m_itemOp->m_file,
					  m_itemOp->m_line,
					  "Instruction node created");
}
