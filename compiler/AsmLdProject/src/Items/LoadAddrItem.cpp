/*
 * LoadAddrItem.cpp
 *
 * "Item implements steps needed to implement "load address" function"
 */

#include "DeviceLayer/LabelTable.h"
#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Utils/BinaryUtils.h"
#include "Utils/ErrorUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/TargetUtils.h"

#include "Items/LoadAddrItem.h"

using namespace std;

//==============================================================================

// Definitions to find desired data in token's raw strings.
#define FLAGS_VAL_IDX 1 // Flags in flag token begin after '%' prefix
#define REG_VAL_IDX   1 // Reg value after '$' prefix

// Definitions to extract bytes from a word.
#define LOWER_BYTE(x) ((x) & 0xFF)
#define UPPER_BYTE(x) (((x) >> 8) & 0xFF)

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

	// (Inform debugging users).
	string dbgStr = " Load Address Item created";
	Printer::getInst()->log(LOG_DEBUG,
			                m_label->m_orignFile,
							m_label->m_originLine,
							dbgStr
						   );
}

//==============================================================================
// Performs analysis and checks available before entire program is parsed.
void LoadAddrItem::doLocalAnalysis(DataModel_t* model) {
	// Check register is valid.
	if (m_reg == nullptr) {
		Printer::getInst()->printAssert("_la function reg not properly set");
		Terminate::getInst()->exit(REASON_ASSERT);
	}
	else {
		// Get common accessed values.
		string rawStr = m_reg->m_rawStr;
		string fileLoc = m_reg->m_orignFile;
		uint32_t lineLoc = m_reg->m_originLine;

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

	// Use of label- record usage.
	model->m_labelTable.useLabel(m_label->m_rawStr);

	// (Inform debugging users).
	string dbgStr = "Label \"" +
			        m_label->m_rawStr +
					"\" referenced";
	Printer::getInst()->log(LOG_DEBUG,
			                m_label->m_orignFile,
							m_label->m_originLine,
							dbgStr);
}

//==============================================================================
// Performs analysis and checks with respect to the entire given program.
void LoadAddrItem::doGlobalAnalysis(DataModel_t* model) {
	// Function translates into 2 LBI instructions- update model to such.
	model->m_numTextBytes += TARGETUTILS_INSTR_SIZE * 2;
}

//==============================================================================
// Confirms if item's contents are placed in text section of binary.
bool LoadAddrItem::isTextContent(void) {
	// Function generates instructions in text section.
	return true;
}

//==============================================================================
// Translates item's values into binary words. Populates model's sections.
void LoadAddrItem::generateBinaryValue(DataModel_t* model) {
	// Requires label resolution to generate- add placeholders.
	vector<uint16_t> placeholders = {0x0000, 0x0000};

	// Update model's text section.
	m_textIdx = model->m_textSection.size();    // save instruction index
	for (uint16_t word : placeholders) {
		model->m_textSection.push_back(word);
	}
}

//==============================================================================
// Resolves any binary generations reliant on label resolution.
void LoadAddrItem::resolveBinaryLabels(DataModel_t& model) {
	// Get the resolved label address.
	uint32_t addr = 0x0000;
	RetErr_e retErr = model.m_labelTable.getAddress(model,
			                                        m_label->m_rawStr,
													addr);
	if (retErr) {
		// By this point, un-paired items should be filtered.
		Printer::getInst()->printAssert("unable to get address for label");
		Terminate::getInst()->exit(REASON_ASSERT);
	}

	// Prepare register value for generation.
	uint32_t regInt;
	StringUtils_asUint(m_reg->m_rawStr.substr(REG_VAL_IDX), &regInt);

	// Generate the "set top byte" instruction.
	int binErr = 0;
	uint16_t firstInstr = 0x0000;
	InstrFields_t firstFields = {
		.m_opcode = INSTR_LBI,
		.m_r1     = regInt,
		.m_imm    = (int32_t)(UPPER_BYTE(addr))
	};
	binErr += BinaryUtils_genInstr(firstInstr, firstFields);

	// Generate the "set bottom byte" instruction.
	uint16_t secondInstr = 0x0000;
	InstrFields_t secondFields = {
		.m_opcode = INSTR_LBI,
		.m_flags  = "s",
		.m_r1     = regInt,
		.m_imm    = (int32_t)(LOWER_BYTE(addr))
	};
	binErr += BinaryUtils_genInstr(secondInstr, secondFields);

	// (ensure generation was successful).
	if (binErr) {
		// Instructions should generate successfully.
		Printer::getInst()->printAssert("unable to generate _la function");
		Terminate::getInst()->exit(REASON_ASSERT);
	}

	// Overwrite placeholders with actual values.
	model.m_textSection.at(m_textIdx)     = firstInstr;
	model.m_textSection.at(m_textIdx + 1) = secondInstr;
}

//==============================================================================
// Returns the generated address of item (if item has address).
RetErr_e LoadAddrItem::getAddress(uint32_t& addr) {
	// Function has (text) address.
	addr = m_textIdx * TARGETUTILS_INSTR_SIZE;

	// Indicate success/presence of address.
	return RET_GOOD;
}

//==============================================================================
// Destructs load address item (along with originally passed scan tokens).
LoadAddrItem::~LoadAddrItem(void) {
	// Hard ownership of data- delete with the item.
	delete m_reg;
	delete m_label;
}
