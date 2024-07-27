/*
 * LabelTable.cpp
 *
 * "Structure used to store and report observed assembly labels"
 */

#include "DeviceLayer/Printer.h"
#include "Items/IBuildItem.h"
#include "Utils/ErrorUtils.h"

#include "DeviceLayer/LabelTable.h"

using namespace std;

//==============================================================================
// TODO
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
// TODO
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
// TODO
void LabelTable::setItem(std::string const label, IBuildItem& item) {
	// Set the label's paired item (if label exists).
	if (m_table.find(label) != m_table.end()) {
		m_table.at(label).m_labelItem = &item;
	}
}

//==============================================================================
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
