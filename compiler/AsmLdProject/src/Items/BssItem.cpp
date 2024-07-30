/*
 * BssItem.cpp
 *
 * "Item defining "uninitialized" data memory"
 */

#include "DeviceLayer/Printer.h"

#include "Items/BssItem.h"

using namespace std;

//==============================================================================
// Constructs label declaration item from valid, parsed tokens.
BssItem::BssItem(std::queue<ScanToken_t*>* tokens) {
	// Begin with known values.
	m_imm    = nullptr;
	m_bssIdx = 0;

	// No need for keyword.
	delete tokens->front();
	tokens->pop();

	// Save the immediate token for further use.
	m_imm = tokens->front();
	tokens->pop();

	// (Inform debugging users).
	string dbgStr = " Bss Item created";
	Printer::getInst()->log(LOG_DEBUG,
			                m_imm->m_orignFile,
							m_imm->m_originLine,
							dbgStr
						   );
}

//==============================================================================
// Performs analysis and checks available before entire program is parsed.
void BssItem::doLocalAnalysis(DataModel_t* model) {
	// TODO
}

//==============================================================================
// Performs analysis and checks with respect to the entire given program.
void BssItem::doGlobalAnalysis(DataModel_t* model) {
	// TODO
}

//==============================================================================
// Translates item's values into binary words. Populates model's sections.
void BssItem::generateBinaryValue(DataModel_t* model) {
	// TODO
}

//==============================================================================
// Resolves any binary generations reliant on label resolution.
void BssItem::resolveBinaryLabels(DataModel_t& model) {
	// TODO
}

//==============================================================================
// Returns the generated address of item (if item has address).
RetErr_e BssItem::getAddress(uint32_t& addr) {
	// Declarations do not have a direct address.
	return RET_FAIL;
}

//==============================================================================
// Destructs label declaration item (along with originally passed tokens).
BssItem::~BssItem(void) {
	// Hard ownership of scan tokens.
	delete m_imm;
}
