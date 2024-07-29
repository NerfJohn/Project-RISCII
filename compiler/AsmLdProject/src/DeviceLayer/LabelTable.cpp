/*
 * LabelTable.cpp
 *
 * "Structure used to store and report observed assembly labels"
 */

#include "DeviceLayer/Printer.h"
#include "Items/IBuildItem.h"
#include "Utils/ErrorUtils.h"
#include "Utils/LabelUtils.h"

#include "DeviceLayer/LabelTable.h"

using namespace std;

//==============================================================================
// Initializes label table with assembler's pre-defined values.
LabelTable::LabelTable(void) {
	// Initialize core table with pre-defined values.
	LabelUtils_initPreDefs(m_table);
}

//==============================================================================
// Defines the given label with a given data (if not already defined).
void LabelTable::defineLabel(DataModel_t& model,
			                 std::string const label,
					         LabelData_t const& data) {
	// Ensure label is not already defined.
	if ((m_table.find(label) != m_table.end()) &&
	    (m_table.at(label).m_isDefined)) {
		// Already defined- report error.
		string errStr = "Multiply defined label \"" +
				        label +
						"\" (defined at " +
						m_table.at(label).m_origFileLoc +
						"/" +
						to_string(m_table.at(label).m_origLineLoc) +
						")";
		Printer::getInst()->log(LOG_ERR,
				                data.m_origFileLoc,
								data.m_origLineLoc,
								errStr);
		ErrorUtils_includeReason(&model, REASON_MULTI_DEFINE);
	}
	else {
		// Not defined- create and/or set flag.
		if (m_table.find(label) == m_table.end()) {
			m_table.insert({label, data});
		}
		m_table.at(label).m_isDefined = true;
	}
}

//==============================================================================
// Marks given label as used in the table. Creates entry as needed.
void LabelTable::useLabel(std::string const label) {
	// Create a new entry if not already present.
	if (m_table.find(label) == m_table.end()) {
		// New entry template.
		LabelData_t entry = {
			.m_isDefined   = false,
			.m_isUsed      = false,
			.m_origFileLoc = "",
			.m_origLineLoc = 0,
			.m_labelItem   = nullptr
		};

		// Add to table.
		m_table.insert({label, entry});
	}

	// Ensure entry (new or old) is used.
	m_table.at(label).m_isUsed = true;
}

//==============================================================================
// Sets the item associated with the given label. Label must exist.
void LabelTable::setItem(std::string const label, IBuildItem& item) {
	// Set the label's paired item (if label exists).
	if (m_table.find(label) != m_table.end()) {
		m_table.at(label).m_labelItem = &item;
	}
}

//==============================================================================
// Runs checks on current entries. Reports any errors or warnings.
void LabelTable::validateTable(DataModel_t& model) {
	// Iterate over each entry to validate the entry.
	map<string, LabelData_t>::iterator entry;
	for (entry = m_table.begin(); entry != m_table.end(); entry++) {
		// Get common information from the entry.
		string labelName = entry->first;
		string fileLoc   = entry->second.m_origFileLoc;
		uint32_t lineLoc = entry->second.m_origLineLoc;

		// First ensure it has been defined.
		if (entry->second.m_isDefined == false) {
			// Entries MUST have definitions.
			string errStr = "Label \"" +
					        labelName +
							"\" was referenced but never defined";
			Printer::getInst()->log(LOG_ERR, errStr);
			ErrorUtils_includeReason(&model, REASON_NO_DEF_LABEL);
		}

		// Next, ensure every label has been used.
		if (entry->second.m_isUsed == false) {
			string warnStr = "Label \"" +
					        labelName +
							"\" is defined but never used";
			Printer::getInst()->log(LOG_WARN, fileLoc, lineLoc, warnStr);
			ErrorUtils_includeReason(&model, REASON_NOUSE_LABEL);
		}
	}
}

//==============================================================================
// Gets the generated address associated with the given label.
RetErr_e LabelTable::getAddress(DataModel_t const& model,
		                        std::string const label,
								uint32_t& addr) {
	// Return code to indicate process success/failure.
	RetErr_e retCode = RET_GOOD; // innocent till guilty

	// Attempt to get the label's address.
	RetErr_e retErr = LabelUtils_getPreDefAddr(model, label, addr);
	if (retErr) {
		// Not a predefined label- check label's item address.
		map<string, LabelData_t>::iterator entry = m_table.find(label);
		if ((entry == m_table.end()) ||
			(entry->second.m_labelItem == nullptr) ||
			(entry->second.m_labelItem->getAddress(addr) == RET_FAIL)) {
			// Wasn't able to get an address.
			retCode = RET_FAIL;
		}
		// else- item->getAddress() call already updated address.
	}
	// else- getPreDefAddr() call already updated address.

	// Return result of process.
	return retCode;
}
