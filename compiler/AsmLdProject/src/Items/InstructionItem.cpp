/*
 * InstructionItem.cpp
 *
 * "Item defining a binary instruction written in assembly"
 */

#include "DeviceLayer/Printer.h"
#include "Utils/ErrorUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/TargetUtils.h"

#include "Items/InstructionItem.h"

using namespace std;

//==============================================================================

// Definitions to find desired data in token's raw strings.
#define FLAGS_VAL_IDX 1 // Flags in flag token begin after '%' prefix
#define REG_VAL_IDX   1 // Reg value after '$' prefix

//==============================================================================
// Constructs instruction item from parsed tokens. Assumes tokens are valid.
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
// Performs analysis and checks available before entire program is parsed.
void InstructionItem::doLocalAnalysis(DataModel_t* model) {
	// Get instruction's type for checking arguments.
	InstrType_e instrType = TargetUtils_asInstr(m_opcode->m_lexTkn);

	// Check flags as applicable.
	if (m_flag != nullptr) {
		// Get common accessed values.
		string rawStr = m_flag->m_rawStr;
		string fileLoc = m_flag->m_orignFile;
		uint32_t lineLoc = m_flag->m_originLine;

		// Warn about possible repeated flags.
		vector<uint8_t> repeats = StringUtils_getRepeatChars(rawStr);
		for (size_t i = 0; i < repeats.size(); i++) {
			string warnStr = string("Flag '")
					         + ((char)(repeats.at(i)))
							 + "' repeats- repeat has no effect";
			Printer::getInst()->log(LOG_WARN, fileLoc, lineLoc, warnStr);
			ErrorUtils_includeReason(model, REASON_REPEAT_FLAG);
		}

		// Next, check each flag is actually valid for instruction.
		string uniqueChars = StringUtils_getNoRepeats(rawStr);
		for (size_t i = FLAGS_VAL_IDX; i < uniqueChars.size(); i++) {
			// Get flag.
			uint8_t flag = uniqueChars.at(i);

			// Error for invalid flag (for this context).
			if (!TargetUtils_isValidFlag(instrType, flag)) {
				string errStr = string("Invalid flag '")
						        + ((char)(flag))
								+ "'- must be from set {"
								+ TargetUtils_getInstrFlags(instrType)
								+ "}";
				Printer::getInst()->log(LOG_ERR, fileLoc, lineLoc, errStr);
				ErrorUtils_includeReason(model, REASON_BAD_FLAG);
			}
		}
	}

	// Check registers as applicable.
	for (ScanToken_t* reg : m_regs) { // any seen tokens for sure exist
		// Get common accessed values.
		string rawStr = reg->m_rawStr;
		string fileLoc = reg->m_orignFile;
		uint32_t lineLoc = reg->m_originLine;

		// Get register value.
		uint32_t regInt;
		string regStr = rawStr.substr(REG_VAL_IDX, rawStr.size() - REG_VAL_IDX);
		bool inRange = StringUtils_asUint(regStr, &regInt);

		// Report any "out-of-bounds" registers.
		if (!inRange || !TargetUtils_isValidReg(regInt)) {
			string errStr = string("Register '")
					        + rawStr
							+ "' must be in range "
							+ TargetUtils_getRegType();
			Printer::getInst()->log(LOG_ERR, fileLoc, lineLoc, errStr);
			ErrorUtils_includeReason(model, REASON_BAD_REG);
		}
	}

	// Check immediate... as applicable.
	if (m_immediate != nullptr) {
		// Get common accessed values.
		string rawStr = m_immediate->m_rawStr;
		string fileLoc = m_immediate->m_orignFile;
		uint32_t lineLoc = m_immediate->m_originLine;

		// Get register value.
		int32_t regInt;
		bool inRange = StringUtils_asInt(rawStr, &regInt);

		// Report any "out-of-bounds" immediate.
		if (!inRange || !TargetUtils_isValidImm(instrType, regInt)) {
			string errStr = string("Immediate '")
					        + rawStr
							+ "' must be in range "
							+ TargetUtils_getImmType(instrType);
			Printer::getInst()->log(LOG_ERR, fileLoc, lineLoc, errStr);
			ErrorUtils_includeReason(model, REASON_BAD_REG);
		}
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
