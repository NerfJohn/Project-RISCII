/*
 * DataNode.cpp: Represents directives specifying global data (data and bss).
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/IsaUtil.h"
#include "Util/ModelUtil.h"

#include "Ds/DataNode.h"

using namespace std;

//==============================================================================
// Constructor called by parser. Builds itself directly from action stack.
DataNode::DataNode(std::stack<ItemToken*>& itemStack) {
	// (Init members.)
	m_itemType = nullptr;
	m_itemVals = {};

	// Parse all items from the stack (ie trusting parser knows its grammar).
	while (itemStack.size() > 0) {
		// (Sanity check item.)
		ItemToken* tkn = itemStack.top();
		if (tkn == nullptr) {Terminate_assert("Null token in DataNode stack");}

		// Take ownership of items based on type (ie REALLY trusting parser...).
		switch (tkn->m_lexTkn) {
			case TOKEN_IMMEDIATE:    m_itemVals.push_front(move(tkn)); break;
			default: /* directive */ m_itemType = move(tkn);           break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// (Sanity check pertinent details.)
	if (m_itemType == nullptr) {Terminate_assert("DataNode has no directive");}

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
					  m_itemType->m_file,
					  m_itemType->m_line,
					  "DataNode created");
}

//==============================================================================
// Runs local analytics and record-keeping on node's data.
void DataNode::doLocalAnalysis(DataModel_t& model, SymTable& syms) {
	// Validate each item on record (ptrs pre-checked by ctor).
	for (ItemToken* itemValue : m_itemVals) {
		// Validate the item based on its underlying type.
		switch(itemValue->m_lexTkn) {
			case TOKEN_IMMEDIATE:
				// Immediate- use common checker.
				this->validateImm(model, *itemValue, *m_itemType);
				break;
			default:
				// Unknown type made it through parsing? bug!
				Terminate_assert("Analyzed unknown item in DataNode");
				break;
		}
	}

	// Claim any open labels.
	this->pairOpenLabels(model, *m_itemType, ADDR_DATA);
}

//==============================================================================
// Adds node to model's overall program data structures.
void DataNode::addToProgram(DataModel_t& model) {
	// Simply add the node to the global program.
	model.m_nodes.push_back(this);
}

//==============================================================================
// Computes address-related data for model and node.
void DataNode::genAddresses(DataModel_t& model) {
	// Total size of node.
	uint32_t totSize = 0;

	// Accumulate the size of each item (ptrs pre-checked by ctor).
	for (ItemToken* item : m_itemVals) {
		// Get size based off underlying type.
		switch (item->m_lexTkn) {
			case TOKEN_IMMEDIATE:
				// Immediate- ALWAYS 1 word in size.
				totSize += ISA_WORD_BYTES;
				break;
			default:
				// Unknown type made it past parsing? bug!
				Terminate_assert("Addressed unknown item in DataNode");
				break;
		}
	}

	// Save size to proper location.
	switch(m_itemType->m_lexTkn) {
		case TOKEN_KW_DATA: model.m_dataSize += totSize;                 break;
		default: Terminate_assert("Addressed DataNode of unknown type"); break;
	}
}

//==============================================================================
// Assembles the node, adding its binary data to the model.
void DataNode::genAssemble(DataModel_t& model) {
	// Only adds to binary if initialized data.
	if (m_itemType->m_lexTkn == TOKEN_KW_DATA) {
		// Assemble based on underlying type (ptrs pre-checked by ctor).
		for (ItemToken* item : m_itemVals) {
			// Get size based off underlying type.
			switch (item->m_lexTkn) {
				case TOKEN_IMMEDIATE:
					// Immediate- use common translation.
					model.m_dataVals.push_back(this->getImmVal(*item));
					break;
				default:
					// Unknown type made it past parsing? bug!
					Terminate_assert("Assembled unknown item in DataNode");
					break;
			}
		}
	}
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
DataNode::~DataNode(void) {
	// Free tokens (ptrs pre-checked by ctor)
	delete m_itemType;
	for (ItemToken* tkn : m_itemVals) {delete tkn;}
}

