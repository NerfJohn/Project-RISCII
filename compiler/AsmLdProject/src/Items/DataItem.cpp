/*
 * DataItem.cpp
 *
 * "Item defining initialized data (in the data section)"
 */

#include "DeviceLayer/LabelTable.h"
#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Utils/ErrorUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/TargetUtils.h"

#include "Items/DataItem.h"

using namespace std;

//==============================================================================
// Constructs label declaration item from valid, parsed tokens.
DataItem::DataItem(std::queue<ScanToken_t*>* tokens) {
	// Begin with known values.
	m_origFile    = "";
	m_origLine    = 0;
	m_vals        = vector<ScanToken_t*>();
	m_labelValIdx = queue<uint32_t>();
	m_dataIdx     = 0;

	// Use the keyword to denote item's location.
	m_origFile = tokens->front()->m_orignFile;
	m_origLine = tokens->front()->m_originLine;
	delete tokens->front();
	tokens->pop();

	// Save different data tokens to item.
	while (!tokens->empty()) {
		// Save token as needed.
		LexToken_e tknType = tokens->front()->m_lexTkn;
		if ((tknType == TOKEN_LCURLY) || (tknType == TOKEN_RCURLY)) {
			// Not data- skip/delete.
			delete tokens->front();
		}
		else {
			// Save/own the data.
			m_vals.push_back(tokens->front());
		}

		// Onto next token.
		tokens->pop();
	}

	// (Inform debugging users).
	string dbgStr = "Data Item created";
	Printer::getInst()->log(LOG_DEBUG,
			                m_origFile,
							m_origLine,
							dbgStr
						   );
}

//==============================================================================
// Performs analysis and checks available before entire program is parsed.
void DataItem::doLocalAnalysis(DataModel_t* model) {
	// Check each value.
	for (ScanToken_t* value : m_vals) {
		// Run appropriate check based on type.
		if (value->m_lexTkn == TOKEN_IMMEDIATE) {
			// Immediate value //

			// Get common accessed values.
			string rawStr = value->m_rawStr;
			string fileLoc = value->m_orignFile;
			uint32_t lineLoc = value->m_originLine;

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
			if (!inRange || !TargetUtils_isValidWord(DATA_WORD_ANY, regInt)) {
				string errStr = string("Immediate '")
						        + rawStr
								+ "' must be in range "
								+ TargetUtils_getWordType(DATA_WORD_ANY);
				Printer::getInst()->log(LOG_ERR, fileLoc, lineLoc, errStr);
				ErrorUtils_includeReason(model, REASON_BAD_IMM);
			}
		}
		else if (value->m_lexTkn == TOKEN_LABEL) {
			// Use of label- record usage.
			model->m_labelTable.useLabel(value->m_rawStr);

			// (Inform debugging users).
			string dbgStr = "Label \"" +
					        value->m_rawStr +
							"\" referenced";
			Printer::getInst()->log(LOG_DEBUG,
					                value->m_orignFile,
									value->m_originLine,
									dbgStr);
		}
		else {
			// Unknown value type- assert!.
			string assertStr = "unknown value type analyzed in data item";
			Printer::getInst()->printAssert(assertStr);
			Terminate::getInst()->exit(REASON_ASSERT);
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
				                m_origFile,
								m_origLine,
								dbgStr);
	}
	model->m_openLabels.clear();
}

//==============================================================================
// Performs analysis and checks with respect to the entire given program.
void DataItem::doGlobalAnalysis(DataModel_t* model) {
	// Add projected size of each value to the model for checks.
	for (ScanToken_t* value : m_vals) {
		// Run appropriate check based on type.
		if (value->m_lexTkn == TOKEN_IMMEDIATE) {
			// Immediates are always resolved to one word.
			model->m_numDataBytes += TARGETUTILS_WORD_SIZE;
		}
		else if (value->m_lexTkn == TOKEN_LABEL) {
			// Labels/addresses are always 16-bits.
			model->m_numDataBytes += TARGETUTILS_WORD_SIZE;
		}
		else {
			// Unknown value type- assert!.
			string assertStr = "unknown value type checked in data item";
			Printer::getInst()->printAssert(assertStr);
			Terminate::getInst()->exit(REASON_ASSERT);
		}
	}
}

//==============================================================================
// Confirms if item's contents are placed in text section of binary.
bool DataItem::isTextContent(void) {
	// Data item explicitly represents data section values.
	return false;
}

//==============================================================================
// Translates item's values into binary words. Populates model's sections.
void DataItem::generateBinaryValue(DataModel_t* model) {
	// Record the index starting "block" of data.
	m_dataIdx = model->m_dataSection.size();

	// Add translated words to model's data section.
	for (ScanToken_t* value : m_vals) {
		// Run appropriate check based on type.
		if (value->m_lexTkn == TOKEN_IMMEDIATE) {
			// Get immediate value as word.
			int32_t immInt = 0;
			StringUtils_asInt(value->m_rawStr, &immInt);
			uint16_t immWord = (uint16_t)(immInt);

			// "Allocate" word in data section.
			model->m_dataSection.push_back(immWord);
		}
		else if (value->m_lexTkn == TOKEN_LABEL) {
			// Add placeholder (of known location) until label can be realized.
			uint16_t placeholder = 0x0000;
			m_labelValIdx.push(model->m_dataSection.size());
			model->m_dataSection.push_back(placeholder);
		}
		else {
			// Unknown value type- assert!.
			string assertStr = "unknown value type translated in data item";
			Printer::getInst()->printAssert(assertStr);
			Terminate::getInst()->exit(REASON_ASSERT);
		}
	}
}

//==============================================================================
// Resolves any binary generations reliant on label resolution.
void DataItem::resolveBinaryLabels(DataModel_t& model) {
	// Search values for labels to resolve.
	for (ScanToken_t* value : m_vals) {
		// Only resolve labels.
		if (value->m_lexTkn == TOKEN_LABEL) {
			// Get the resolved label address.
			uint32_t addr = 0x0000;
			RetErr_e retErr = model.m_labelTable.getAddress(model,
					                                        value->m_rawStr,
															addr);
			if (retErr) {
				// By this point, un-paired items should be filtered.
				Printer::getInst()->printAssert("unable to get label address");
				Terminate::getInst()->exit(REASON_ASSERT);
			}

			// Replace placeholder with address.
			model.m_dataSection[m_labelValIdx.front()] = addr;

			// Pop index to get next label's index.
			m_labelValIdx.pop();

			// (Inform debugging users).
			string dbgStr = "Label \"" +
					        value->m_rawStr +
							"\" resolved to address " +
							to_string(addr);
			Printer::getInst()->log(LOG_DEBUG,
					                value->m_orignFile,
									value->m_originLine,
									dbgStr);
		}
	}
}

//==============================================================================
// Returns the generated address of item (if item has address).
RetErr_e DataItem::getAddress(uint32_t& addr) {
	// Data is addressable- compute address based on word's index.
	addr = m_dataIdx * TARGETUTILS_WORD_SIZE;
	return RET_GOOD;
}

//==============================================================================
// Destructs label declaration item (along with originally passed tokens).
DataItem::~DataItem(void) {
	// Hard ownership of scan tokens.
	for (ScanToken_t* val : m_vals) {delete val;}
}
