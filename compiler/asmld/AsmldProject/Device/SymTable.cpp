/*
 * SymTable.cpp: Table for declaring/modifying/referencing label symbols.
 */

#include "Device/Terminate.h"

#include "Device/SymTable.h"

using namespace std;

//==============================================================================
// Adds given symbol to table (if key has no assigned symbol).
RetErr_e SymTable::addSym(std::string const& key, Symbol_t* sym) {
	// Result of the process (pessimistic).
	RetErr_e retErr = RET_ERR_ERROR;

	// (Input symbol ptrs CANNOT be null.)
	IF_NULL(sym, "Adding null symbol");

	// TODO- check reserved labels- this XOR symbols below

	// Add symbol if key hasn't already been taken.
	if (m_table.find(key) == m_table.end()) {
		// Save symbol.
		m_table.insert({key, sym});

		// TODO- trip flag if "start" label was added"

		// "All good in the [SymTable]."
		retErr = RET_ERR_NONE;
	}

	// Return result of the process.
	return retErr;
}

//==============================================================================
// Finds symbol based on key.
RetErr_e SymTable::getSym(std::string const& key, Symbol_t*& sym) {
	// Result of the process (pessimistic).
	RetErr_e retErr = RET_ERR_ERROR;

	// TODO- check reserved labels- this XOR symbols below

	// Update reference ONLY IF key is taken.
	if (m_table.find(key) != m_table.end()) {
		// Retrieve symbol.
		sym = m_table.at(key);

		// (Sanity check ptr.)
		IF_NULL(sym, "Got null symbol");

		// "All good in the [SymTable]."
		retErr = RET_ERR_NONE;
	}

	// Return result of the process.
	return retErr;
}

//==============================================================================
// Populates the given vector with table's symbols.
void SymTable::asList(std::vector<Symbol_t*>& syms) {
	// Overwrite vector with map's contents.
	syms.clear();
	for (auto entry : m_table) {
		IF_NULL(entry.second, "asList() found null symbol");
		syms.push_back(entry.second);
	}
}

