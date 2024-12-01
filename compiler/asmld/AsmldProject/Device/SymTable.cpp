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
		newSym->m_name    = name;
		newSym->m_file    = labelTkn.m_file;
		newSym->m_line    = labelTkn.m_line;
		newSym->m_numRefs = 1;               // just created- only reference

		// Save symbol to table.
		m_table.insert({name, newSym});

		// Return reference to new symbol.
		sym = newSym;
	}

	// Return result of the process.
	return retErr;
}

//==============================================================================
// Adds the given symbol to the table (if the nameis not already taken).
RetErr_e SymTable::addSym(std::string const& name, Symbol_t* sym) {
	// Result to return (pessimistic).
	RetErr_e retErr = RET_ERR_ERROR;

	// Only add actual symbols- bug otherwise.
	if (sym == nullptr) {Terminate_assert("SymTable- adding null symbol");}

	// Add symbol to table if key isn't already used.
	if (m_table.find(name) == m_table.end()) {
		// Save symbol to table.
		m_table.insert({name, sym});

		// Good run- return as such.
		retErr = RET_ERR_NONE;
	}

	// Return result of the process.
	return retErr;
}

//==============================================================================
// Gets a symbol based on the given name.
RetErr_e SymTable::getSym(std::string const& name, Symbol_t*& sym) {
	// Result to return (pessimistic).
	RetErr_e retErr = RET_ERR_ERROR;

	// Populate the pointer if found.
	if (m_table.find(name) != m_table.end()) {
		// Get symbol.
		sym = m_table.at(name);
		if (sym == nullptr) {Terminate_assert("getSym() on null symbol");}

		// Symbol found successfully.
		retErr = RET_ERR_NONE;
	}

	// Return result of the process.
	return retErr;
}
