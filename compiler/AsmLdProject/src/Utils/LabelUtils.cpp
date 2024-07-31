/*
 * LabelUtils.cpp
 *
 * "Helper functions for working with (special) labels"
 */

#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Items/IBuildItem.h"
#include "Utils/TargetUtils.h"

#include "Utils/LabelUtils.h"

using namespace std;

//==============================================================================

// Definitions for pre-defined label names.
#define START_LABEL string("__START") // starting instruction of program
#define BSS_LABEL   string("__BSS")   // first word address of bss section
#define FREE_LABEL  string("__FREE")  // first word address of free memory
#define SIZE_LABEL  string("__SIZE")  // byte size of the binary image

//==============================================================================
// Initializes string->LabelData_t map with pre-defined label information.
RetErr_e LabelUtils_initPreDefs(std::map<std::string, LabelData_t>& table) {
	// Return code to indicate process success/failure.
	RetErr_e retCode = RET_GOOD; // innocent till guilty

	// Add starting label- "used" by assembler, but must be user defined.
	LabelData_t startEntry = {
		.m_isDefined   = false,
		.m_isUsed      = true,
		.m_origFileLoc = "",
		.m_origLineLoc = 0,
		.m_labelItem   = nullptr
	};
	table.insert({START_LABEL, startEntry});

	// Add "sentinel" labels- defined/used by assembler (for user use).
	LabelData_t sentinelEntry = {
		.m_isDefined   = true,
		.m_isUsed      = true,
		.m_origFileLoc = "",
		.m_origLineLoc = 0,
		.m_labelItem   = nullptr
	};
	table.insert({BSS_LABEL, sentinelEntry});
	table.insert({FREE_LABEL, sentinelEntry});
	table.insert({SIZE_LABEL, sentinelEntry});

	// Return result of process.
	return retCode;
}

//==============================================================================
// Simple getter for program start label name.
std::string LabelUtils_getStartName(void) {
	// Simple getter- simple return.
	return START_LABEL;
}

//==============================================================================
// Checks if pre-defined "program start" label is valid for translation.
bool LabelUtils_isStartValid(std::map<std::string, LabelData_t>& table) {
	// Returned boolean- indicating validity of start label.
	bool retBool = true; // assume valid till proven otherwise

	// Get the item associate with the start label.
	if (table.find(START_LABEL) == table.end()) {
		// Table's pre-defs weren't initialized? Design flaw!
		string assertStr = START_LABEL + " missing from table";
		Printer::getInst()->printAssert(assertStr);
		Terminate::getInst()->exit(REASON_ASSERT);
	}
	IBuildItem* startItem = table.at(START_LABEL).m_labelItem;

	// Start item must exist and be within text section.
	if ((startItem == nullptr) || !startItem->isTextContent()) {
		// NOT a valid item for start label.
		retBool = false;
	}

	// Return result of checks.
	return retBool;
}

//==============================================================================
// Gets address associated with an assembler defined label.
RetErr_e LabelUtils_getPreDefAddr(DataModel_t const& model,
		                          std::string const label,
								  uint32_t& addr) {
	// Return code to indicate process success/failure.
	RetErr_e retCode = RET_GOOD; // innocent till guilty

	// Set the address based on the pre-defined label.
	if (label.compare(START_LABEL) == 0) {
		// Start label marks first word address in text address space.
		addr = 0x0000;
	}
	else if (label.compare(BSS_LABEL) == 0) {
		// Bss section is just after data section in data address space.
		addr = model.m_numDataBytes;
	}
	else if (label.compare(FREE_LABEL) == 0) {
		// Free memory is after data and bss sections in data address space.
		addr = model.m_numDataBytes + model.m_numBssBytes;
	}
	else if (label.compare(SIZE_LABEL) == 0) {
		// Binary consists of text and data sections + metadata.
		addr = model.m_numTextBytes +
			   model.m_numDataBytes +
			   (TARGETUTILS_METADATA_SIZE);
	}
	else {
		// Label is not a pre-defined label- failure.
		retCode = RET_FAIL;
	}

	// Return result of process.
	return retCode;
}

