/*
 * LoadAddrItem.cpp
 *
 * "Item implements steps needed to implement "load address" function"
 */

#include "Items/LoadAddrItem.h"

using namespace std;

//==============================================================================
// Constructs load addr item from parsed tokens. Assumes tokens are valid.
LoadAddrItem::LoadAddrItem(std::queue<ScanToken_t*>* tokens) {
	// Begin with known values.
	m_reg     = nullptr;
	m_label   = nullptr;
	m_textIdx = 0;

	// Throw away keyword- not needed for record.
	delete tokens->front();
	tokens->pop();

	// Record the register to use for function operation.
	m_reg = tokens->front();
	tokens->pop();

	// Record label to use/reference for function operation.
	m_label = tokens->front();
	tokens->pop();
}

//==============================================================================
// Performs analysis and checks available before entire program is parsed.
void LoadAddrItem::doLocalAnalysis(DataModel_t* model) {
	// TODO
}

//==============================================================================
// Performs analysis and checks with respect to the entire given program.
void LoadAddrItem::doGlobalAnalysis(DataModel_t* model) {
	// TODO
}

//==============================================================================
// Translates item's values into binary words. Populates model's sections.
void LoadAddrItem::generateBinaryValue(DataModel_t* model) {
	// TODO
}

//==============================================================================
// Returns the generated address of item (if item has address).
RetErr_e LoadAddrItem::getAddress(uint32_t& addr) {
	// TODO
	return RET_GOOD;
}

//==============================================================================
// Destructs load address item (along with originally passed scan tokens).
LoadAddrItem::~LoadAddrItem(void) {
	// Hard ownership of data- delete with the item.
	delete m_reg;
	delete m_label;
}
