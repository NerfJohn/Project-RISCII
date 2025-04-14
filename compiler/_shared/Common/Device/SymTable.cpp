/*
 * SymTable.cpp: Standard implementation of scoped name/symbol pairs.
 */

#include "Common/Device/Terminate.h"

#include "Common/Device/SymTable.h"

using namespace std;

//==============================================================================
// Adds new scope to symbol table. Scope becomes new "most local".
void SymTable::scopePush(void) {
	// Add a new, empty map to the back/"lower scope".
	m_table.push_back({});
}

//==============================================================================
// Removes "most local" scope from table.
void SymTable::scopePop(void) {
	// Can't pop- compiler design issue!
	if (m_table.size() == 0) {Terminate::inst().assert("scopePop() empty");}
	
	// Otherwise- pop last map (SymTable is NOT in charge of symbol ownership).
	m_table.pop_back();
}

//==============================================================================
// Adds symbol (tied to key) in "most local" scope.
RetErr_e SymTable::addLocal(std::string const& name, void* symbol) {
	// Value to return.
	RetErr_e retErr = RET_ERR_ERROR;
	
	// Can't save nullptr (due to get() implementation).
	IF_NULL(symbol, "addLocal() nullptr");

	// Get local table's index.
	int idx = m_table.size() - 1;
	if (idx < 0) {Terminate::inst().assert("addLocal() no scope");}
	
	// Add symbol if not already taken.
	if (m_table[idx].find(name) == m_table[idx].end()) {
		// Save symbol.
		m_table[idx].insert({name, symbol});

		// "All good in the [SymTable]."
		retErr = RET_ERR_NONE;
	}

	// Return result of operation.
	return retErr;
}

//==============================================================================
// Adds symbol (tied to key) in "least local" scope.
RetErr_e SymTable::addGlobal(std::string const& name, void* symbol) {
	// Value to return.
	RetErr_e retErr = RET_ERR_ERROR;
	
	// Can't save nullptr (due to get() implementation).
	IF_NULL(symbol, "addGlobal() nullptr");

	// Make sure "global" scope exists.
	if (m_table.size() == 0) {Terminate::inst().assert("addGlobal() no scope");}
	
	// Add symbol if not already taken.
	if (m_table[0].find(name) == m_table[0].end()) {
		// Save symbol.
		m_table[0].insert({name, symbol});

		// "All good in the [SymTable]."
		retErr = RET_ERR_NONE;
	}
	
	// Return result of operation.
	return retErr;
}

//==============================================================================
// Get symbol (pointer) associated with key. Biased towards local.
void* SymTable::get(std::string const& name) {
	// Value to return.
	void* retPtr = nullptr;
	
	// Search for symbol- starting from the lowest scope.
	for (int i = m_table.size() - 1; i >= 0; i--) {
		// Stop search (successfully) if found.
		if (m_table[i].find(name) != m_table[i].end()) {
			// Update ptr.
			retPtr = m_table[i].at(name);
			
			// Stop searching.
			break;
		}
	}
	
	// Return result of operation.
	return retPtr;
}

//==============================================================================
// Gets the total number of entries across all scopes.
size_t SymTable::size(void) {
	// Size = sum of each scope's size.
	size_t retSize = 0;
	for (map<string, void*> scope : m_table) {retSize += scope.size();}

	// Voila! the size.
	return retSize;
}
