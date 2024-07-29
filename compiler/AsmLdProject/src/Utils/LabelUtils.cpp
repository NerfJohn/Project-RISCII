/*
 * LabelUtils.cpp
 *
 *  Created on: Jul 27, 2024
 *      Author: John
 */

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

