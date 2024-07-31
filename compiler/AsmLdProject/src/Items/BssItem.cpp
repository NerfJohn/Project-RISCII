/*
 * BssItem.cpp
 *
 * "Item defining "uninitialized" data memory"
 */

#include "DeviceLayer/LabelTable.h"
#include "DeviceLayer/Printer.h"
#include "Utils/ErrorUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/TargetUtils.h"

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
	// Check immediate... as applicable.
	if (m_imm != nullptr) {
		// Get common accessed values.
		string rawStr = m_imm->m_rawStr;
		string fileLoc = m_imm->m_orignFile;
		uint32_t lineLoc = m_imm->m_originLine;

		// Get immediate value.
		int32_t regInt = 0;
		bool inRange = StringUtils_asInt(rawStr, &regInt);

		// (Inform debugging users).
		string dbgStr = string("\"") +
				        rawStr +
						"\" -toInt-> " +
						to_string(regInt);
		Printer::getInst()->log(LOG_DEBUG, fileLoc, lineLoc, dbgStr);

		// Report any "out-of-bounds" immediate.
		if (!inRange || !TargetUtils_isValidWord(DATA_WORD_UNSIGNED, regInt)) {
			string errStr = string("Immediate '")
					        + rawStr
							+ "' must be in range "
							+ TargetUtils_getWordType(DATA_WORD_UNSIGNED);
			Printer::getInst()->log(LOG_ERR, fileLoc, lineLoc, errStr);
			ErrorUtils_includeReason(model, REASON_BAD_IMM);
		}
	}

	// "Claim" any open labels- defining where the label points to.
	for (string labelName : model->m_openLabels) {
		// Claim the label.
		model->m_labelTable.setItem(labelName, *this);

		// (Inform debugging users).
		string dbgStr = "Label \"" +
				        labelName +
						"\" paired";
		Printer::getInst()->log(LOG_DEBUG,
				                m_imm->m_orignFile,
								m_imm->m_originLine,
								dbgStr);
	}
	model->m_openLabels.clear();
}

//==============================================================================
// Performs analysis and checks with respect to the entire given program.
void BssItem::doGlobalAnalysis(DataModel_t* model) {
	// Get immediate value as integer.
	int32_t immInt = 0;
	StringUtils_asInt(m_imm->m_rawStr, &immInt);

	// Allocated space is rounded UP to the nearest word size.
	if (immInt % 2) {
		// (Inform debugging users.)
		string dbgStr = "Rounding immediate \"" +
				        to_string(immInt) +
						"\" to word boundary (ie = " +
						to_string(immInt + 1) +
						")";
		Printer::getInst()->log(LOG_DEBUG,
				                m_imm->m_orignFile,
								m_imm->m_originLine,
								dbgStr);

		// Round up.
		immInt++;
	}
	model->m_numBssBytes += immInt;
}

//==============================================================================
// Confirms if item's contents are placed in text section of binary.
bool BssItem::isTextContent(void) {
	// Bss section is within data address space- NOT text section!
	return false;
}

//==============================================================================
// Translates item's values into binary words. Populates model's sections.
void BssItem::generateBinaryValue(DataModel_t* model) {
	// Get immediate value as integer (rounded UP to word address.
	int32_t immInt = 0;
	StringUtils_asInt(m_imm->m_rawStr, &immInt);
	if (immInt % 2) {immInt++;}

	// "Allocate" RAM memory, recording starting address of block.
	m_bssIdx = model->m_bssSection;
	model->m_bssSection += immInt;
}

//==============================================================================
// Resolves any binary generations reliant on label resolution.
void BssItem::resolveBinaryLabels(DataModel_t& model) {
	// No additional generation needed //
}

//==============================================================================
// Returns the generated address of item (if item has address).
RetErr_e BssItem::getAddress(uint32_t& addr) {
	// Bss section is addressable- return address.
	addr = m_bssIdx;

	// Return success for present address.
	return RET_GOOD;
}

//==============================================================================
// Destructs label declaration item (along with originally passed tokens).
BssItem::~BssItem(void) {
	// Hard ownership of scan tokens.
	delete m_imm;
}
