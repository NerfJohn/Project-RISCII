/*
 * SymbolTable.cpp
 */

#include "SymbolTable.h"

using namespace std;

// Attempts to create and return a new symbol for the given name.
Symbol* SymbolTable::addSym(std::string name) {
	// Ensure at least one scope is present.
	if (m_table.size() == 0) {
		m_table.push_back({});
	}

	// Most local scope = top-most index.
	unordered_map<string,Symbol*>* localScope = &m_table[m_table.size() - 1];

	// Check if symbol already exists.
	if (localScope->find(name) != localScope->end()) {
		// Symbol already exists- return null to signal this.
		return nullptr;
	}

	// Confirmed new Symbol- create and save in table.
	Symbol* newSym = new Symbol();
	(*localScope)[name] = newSym;

	// Return ptr of new symbol to signal success.
	return newSym;
}

// Getter for fetching existing local name/symbol pair.
Symbol* SymbolTable::getLocal(std::string name) {
	// Ensure at least one scope is present.
	if (m_table.size() == 0) {
		m_table.push_back({});
	}

	// Most local scope = top-most index.
	unordered_map<string,Symbol*>* localScope = &m_table[m_table.size() - 1];

	// Return find result (default to nullptr if NOT found).
	Symbol* retPtr;
	if (localScope->find(name) == localScope->end()) {retPtr = nullptr;}
	else {retPtr = (*localScope)[name];}
	return retPtr;
}

// Getters for fetching existing general name/symbol pair.
Symbol* SymbolTable::getGlobal(std::string name) {
	// Ensure at least one scope is present.
	if (m_table.size() == 0) {
		m_table.push_back({});
	}

	// Check each scope (in reverse- LIFO approach for scope locality).
	for (int i = m_table.size(); i > 0; i--) {
		// Get the current scope.
		unordered_map<string,Symbol*>* curScope = &m_table[i - 1];

		// Return most localized scope if symbol is found.
		if (curScope->find(name) != curScope->end()) {
			return (*curScope)[name];
		}
	}

	// No scope with desired symbol- return nullptr to signal failure.
	return nullptr;
}
