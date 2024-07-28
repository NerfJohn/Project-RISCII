/*
 * LabelDeclItem.cpp
 *
 * "Item defining a declaration of a labeled address/item"
 */

#include "DeviceLayer/LabelTable.h"
#include "DeviceLayer/Printer.h"
#include "DomainLayer/LabelData_t.h"
#include "Utils/ErrorUtils.h"

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
	// Define the new label.
	LabelData_t newEntry = {
		.m_isDefined   = true,
		.m_isUsed      = false,
		.m_origFileLoc = m_label->m_orignFile,
		.m_origLineLoc = m_label->m_originLine,
		.m_labelItem   = nullptr
	};
	model->m_labelTable.defineLabel(*model, m_label->m_rawStr, newEntry);

	// (Inform debugging users).
	string dbgStr = "Label \"" +
			        m_label->m_rawStr +
					"\" defined";
	Printer::getInst()->log(LOG_DEBUG,
			                m_label->m_orignFile,
							m_label->m_originLine,
							dbgStr);

	// New label declared- needs to be assigned an item.
	model->m_openLabels.push_back(m_label->m_rawStr);
}

//==============================================================================
// Performs analysis and checks with respect to the entire given program.
void LabelDeclItem::doGlobalAnalysis(DataModel_t* model) {
	// No global analysis- done with respect to symbol table //
}

//==============================================================================
// Translates item's values into binary words. Populates model's sections.
void LabelDeclItem::generateBinaryValue(DataModel_t* model) {
	// No direct binary value //
}

//==============================================================================
// Resolves any binary generations reliant on label resolution.
void LabelDeclItem::resolveBinaryLabels(DataModel_t& model) {
	// Instructions do not require additional generation //
}

//==============================================================================
// Returns the generated address of item (if item has address).
RetErr_e LabelDeclItem::getAddress(uint32_t& addr) {
	// Declarations do not have a direct address.
	return RET_FAIL;
}

//==============================================================================
// Destructs label declaration item (along with originally passed tokens).
LabelDeclItem::~LabelDeclItem(void) {
	// Hard ownership of scan tokens.
	delete m_label;
}
