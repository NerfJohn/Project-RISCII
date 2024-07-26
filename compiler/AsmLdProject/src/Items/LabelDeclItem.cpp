/*
 * LabelDeclItem.cpp
 *
 * "Item defining a declaration of a labeled address/item"
 */

#include "DeviceLayer/Printer.h"

#include "Items/LabelDeclItem.h"

using namespace std;

//==============================================================================
// Constructs label declaration item from valid, parsed tokens.
LabelDeclItem::LabelDeclItem(std::queue<ScanToken_t*>* tokens) {
	// Save the label token for further use.
	m_label = tokens->front();
	tokens->pop();

	// Delete/empty rest of tokens (just lexing/grammar aids).
	while (!tokens->empty()) {
		delete tokens->front();
		tokens->pop();
	}

	// (Inform debugging users).
	string dbgStr = "Label Declaration Item created";
	Printer::getInst()->log(LOG_DEBUG,
			                m_label->m_orignFile,
							m_label->m_originLine,
							dbgStr
						   );
}

//==============================================================================
// Performs analysis and checks available before entire program is parsed.
void LabelDeclItem::doLocalAnalysis(DataModel_t* model) {
	// TODO
}

//==============================================================================
// Performs analysis and checks with respect to the entire given program.
void LabelDeclItem::doGlobalAnalysis(DataModel_t* model) {
	// TODO
}

//==============================================================================
// Translates item's values into binary words. Populates model's sections.
void LabelDeclItem::generateBinaryValue(DataModel_t* model) {
	// TODO
}

//==============================================================================
// Destructs label declaration item (along with originally passed tokens).
LabelDeclItem::~LabelDeclItem(void) {
	// Hard ownership of scan tokens.
	delete m_label;
}
