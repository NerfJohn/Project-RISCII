/*
 * SymTable.cpp: Table for declaring/modifying/referencing label symbols.
 */

#include "Device/Terminate.h"

#include "Device/SymTable.h"

using namespace std;

//==============================================================================

// Common values used in defining shared labels.
#define PREDEF_SYM_FILE ("asmld_predef")
#define PREDEF_SYM_LINE (0)

//==============================================================================

// Boot definitions of shared symbols (prior to init).
bool     SymTable::s_symsInit = false;
Symbol_t SymTable::s_symBSS   = {};
Symbol_t SymTable::s_symFREE  = {};
Symbol_t SymTable::s_symSIZE  = {};

//==============================================================================
// Constructor to ensure shared, pre-defined labels are defined.
SymTable::SymTable(void) {
	// Initialize shared symbols ONCE (g++ not liking "designated" inits).
	if (s_symsInit == false) {
		// Init "__BSS" symbol.
		s_symBSS.m_name      = SYM_BSS_NAME;
		s_symBSS.m_file      = PREDEF_SYM_FILE;
		s_symBSS.m_line      = PREDEF_SYM_LINE;
		s_symBSS.m_isGlobal  = true;
		s_symBSS.m_numRefs++;

		// Init "__FREE" symbol.
		s_symFREE.m_name     = SYM_FREE_NAME;
		s_symFREE.m_file     = PREDEF_SYM_FILE;
		s_symFREE.m_line     = PREDEF_SYM_LINE;
		s_symFREE.m_isGlobal = true;
		s_symFREE.m_numRefs++;

		// Init "__SIZE" symbol.
		s_symSIZE.m_name     = SYM_SIZE_NAME;
		s_symSIZE.m_file     = PREDEF_SYM_FILE;
		s_symSIZE.m_line     = PREDEF_SYM_LINE;
		s_symSIZE.m_isGlobal = true;
		s_symSIZE.m_numRefs++;

		// "Pre-def" done.
		s_symsInit = true;
	}
}

//==============================================================================
// Adds given symbol to table (if key has no assigned symbol).
RetErr_e SymTable::addSym(std::string const& key, Symbol_t* sym) {
	// Result of the process (pessimistic).
	RetErr_e retErr = RET_ERR_ERROR;

	// (Input symbol ptrs CANNOT be null.)
	IF_NULL(sym, "Adding null symbol");

	// Modify "start" symbol to always be global + referenced.
	if (key.compare(SYM_START_NAME) == 0) {
		sym->m_isGlobal = true;
		sym->m_numRefs++;
	}

	// NEVER add a shared, pre-defined symbol.
	if ((key.compare(SYM_BSS_NAME)  == 0) ||
		(key.compare(SYM_FREE_NAME) == 0) ||
		(key.compare(SYM_SIZE_NAME) == 0)) { /* already RET_ERR_ERROR */}

	// Otherwise, add symbol if key hasn't already been taken.
	else if (m_table.find(key) == m_table.end()) {
		// Save symbol.
		m_table.insert({key, sym});

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

	// Re-direct request if requesting a shared symbol.
	if (key.compare(SYM_BSS_NAME) == 0) {
		sym    = &s_symBSS;
		retErr = RET_ERR_NONE;
	}
	else if (key.compare(SYM_FREE_NAME) == 0) {
		sym    = &s_symFREE;
		retErr = RET_ERR_NONE;
	}
	else if (key.compare(SYM_SIZE_NAME) == 0) {
		sym    = &s_symSIZE;
		retErr = RET_ERR_NONE;
	}

	// Otherwise, update reference ONLY IF key is taken.
	else if (m_table.find(key) != m_table.end()) {
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

