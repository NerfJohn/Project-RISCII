/*
 * SymTable.cpp: Table for declaring/modifying/referencing label symbols.
 */

#include "Device/Terminate.h"

#include "Device/SymTable.h"

using namespace std;

//==============================================================================
// Creates a new symbol with a label- returning a link to symbol info.
RetErr_e SymTable::define(ItemToken& labelTkn, Symbol_t*& sym) {
	// Result to return (optimistic).
	RetErr_e retErr = RET_ERR_NONE;

	// (Sanity check item is a label.)
	if (labelTkn.m_lexTkn != TOKEN_LABEL) {Terminate_assert("No-label symbol");}

	// Act based on presence of a symbol with the same name.
	string name = labelTkn.m_rawStr;
	if (m_table.find(name) != m_table.end()) {
		// Symbol name already used- return its reference instead.
		sym = m_table.at(name);
		retErr = RET_ERR_ERROR;              // "already defined" error
	}
	else {
		// Symbol name not defined- create new symbol.
		Symbol_t* newSym = new Symbol_t();
		if (newSym == nullptr) {Terminate_assert("Couldn't create symbol");}

		// Fill in symbol with given data.
		newSym->m_name = name;
		newSym->m_file = labelTkn.m_file;
		newSym->m_line = labelTkn.m_line;

		// Save symbol to table.
		m_table.insert({name, newSym});

		// Return reference to new symbol.
		sym = newSym;
	}

	// Return result of the process.
	return retErr;
}
