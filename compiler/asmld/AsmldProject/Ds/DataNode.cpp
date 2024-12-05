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
// Helper function to determine bytes to allocate using immediate.
uint32_t DataNode::allocImm(DataModel_t& model,
		                    ItemToken const& imm,
				            ItemToken const& type) {
	// Number of bytes to allocate/return.
	uint32_t retNum = 0;

	// Allocation depends on type of directive.
	switch (type.m_lexTkn) {
		case TOKEN_KW_DATA: retNum += ISA_WORD_BYTES; break; // init as word
		default:
			// Unknown directive type. Bug!
			Terminate_assert("allocImm()unknown directive");
	}

	// Return resulting number of bytes to allocate.
	return retNum;
}

//==============================================================================
// Constructor called by parser. Builds itself directly from action stack.
DataNode::DataNode(std::stack<ItemToken*>& itemStack) {
	// (Init members.)
	m_reqType = nullptr;
	m_optVals = {};
	m_syms    = {};

	// Parse all items from the stack.
	while (itemStack.size() > 0) {
		// Get next item.
		ItemToken* item = itemStack.top();
		IF_NULL(item, "DataNode() with null item");

		// "Take" item in appropriate spot (REALLY trusting parser, here).
		switch (item->m_lexTkn) {
			case TOKEN_LABEL:
			case TOKEN_IMMEDIATE:    m_optVals.push_back(item); break;
			default: /* directive */ m_reqType = item;          break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// Ensure required items are present.
	IF_NULL(m_reqType, "DataNode() without directive");

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
			          m_reqType->m_file,
					  m_reqType->m_line,
					  "DataNode created");
}

//==============================================================================
// Analyzes node- validating local arguments/symbols.
void DataNode::localAnalyze(DataModel_t& model, SymTable& table){
	// Validate each item.
	for (ItemToken* item : m_optVals) {
		// Validate based on underlying type.
		IF_NULL(item, "analyze() null data item");
		switch(item->m_lexTkn) {
			case TOKEN_LABEL:
				// Label- no actions to take as of yet.
				break;
			case TOKEN_IMMEDIATE:
				// Immediate- use common checker.
				this->getImm(model, *item, *m_reqType); // ignore val for now
				break;
			default:
				// Unknown type made it through parsing? bug!
				Terminate_assert("analyze() unknown data item");
				break;
		}
	}

	// Claim any open labels- using appropriate space.
	AddrSpace_e space = ADDR_INVALID;
	switch(m_reqType->m_lexTkn) {
		case TOKEN_KW_DATA: space = ADDR_DATA; break;
		default: Terminate_assert("analyze() unknown data directive"); break;
	}
	this->pairLabels(model, *m_reqType, space);
}

//==============================================================================
// Handles local links/symbols- modifying and linking to local symbols.
void DataNode::localLink(DataModel_t& model, SymTable& table) {
	// Create symbol links for each label present.
	for (ItemToken* item : m_optVals) {
		// Create/link symbol for label.
		IF_NULL(item, "localLink() with null data item");
		if (item->m_lexTkn == TOKEN_LABEL) {
			Symbol_t* sym = nullptr;            // create
			this->linkLocal(table, *item, sym); // link
			m_syms.push_back(sym);              // save
		}
	}
}

//==============================================================================
// Handles global links/symbols- finishing overall symbol linkage.
void DataNode::globalLink(DataModel_t& model) {
	// Counter to track symbol currently being worked on.
	size_t symIdx = 0;

	// Link remaining labels within the itms.
	for (ItemToken* item : m_optVals) {
		// Link label.
		IF_NULL(item, "globalLink() with null data item");
		if (item->m_lexTkn == TOKEN_LABEL) {
			// Link (as needed- function skips solid links).
			this->linkGlobal(model, *item, m_syms[symIdx]);

			// To next symbol to update.
			symIdx++;
		}
	}
}

//==============================================================================
// Analyze program- generating addresses for each symbol.
void DataNode::imageAddress(DataModel_t& model) {
	// Total byte size of node.
	uint32_t totSize = 0;

	// Counter to track symbol currently being worked on.
	size_t symIdx = 0;

	// Accumulate the size of each item.
	for (ItemToken* item : m_optVals) {
		// Get size based off underlying type.
		IF_NULL(item, "address() null data item");
		switch (item->m_lexTkn) {
			case TOKEN_LABEL:
				// Label- ALWAYS word size.
				totSize += ISA_WORD_BYTES;
				symIdx++;
				break;
			case TOKEN_IMMEDIATE:
				// Immediate- size based on directive type.
				totSize += this->allocImm(model, *item, *m_reqType);
				break;
			default:
				// Unknown type made it past parsing? bug!
				Terminate_assert("address() unknown data item");
				break;
		}
	}

	// Save size to proper location.
	switch(m_reqType->m_lexTkn) {
		case TOKEN_KW_DATA: model.m_dataSize += totSize;               break;
		default: Terminate_assert("address() unknown data directive"); break;
	}
}

//==============================================================================
// Assembles program- generating binary values in the data model.
void DataNode::imageAssemble(DataModel_t& model) {
	// Handy uint16_t for intermediate computing.
	uint16_t binData = 0;

	// Counter to track symbol currently being worked on.
	size_t symIdx = 0;

	// Only initialized data creates image data.
	if (m_reqType->m_lexTkn == TOKEN_KW_DATA) {
		// Assemble each item.
		for (ItemToken* item : m_optVals) {
			// Translate based on underlying type.
			IF_NULL(item, "assemble()null data item");
			switch (item->m_lexTkn) {
				case TOKEN_LABEL:
					// Label- add resolved address.
					IF_NULL(m_syms[symIdx], "assemble() null symbol data item");
					model.m_dataVals.push_back(m_syms[symIdx]->m_addr);
					symIdx++;
					break;
				case TOKEN_IMMEDIATE:
					// Immediate- extract/save word.
					binData = (uint16_t)this->getImm(model, *item, *m_reqType);
					model.m_dataVals.push_back(binData);
					break;
				default:
					// Unknown type made it past parsing? Bug!
					Terminate_assert("assemble() unknown data item");
			}
		}
	}
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
DataNode::~DataNode(void) {
	// Free required items.
	delete m_reqType;

	// Free other items as necessary.
	for (ItemToken* item : m_optVals) {if (item == nullptr) {delete item;}}
}

