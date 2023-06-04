/*
 * SymbolTable.h
 */

#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include <string>
#include <unordered_map>
#include <vector>
#include "Symbol.h"

/*
 * Class that handles Symbol Table operations (and Symbol allocations).
 *
 * Class acts as a symbol table, used to create, lookup, and keep track of
 * name/symbol pairs while semantically checking am Abstract Syntax Tree (AST).
 * Symbol Table dynamically allocates symbols as pairs are added, allowing the
 * paired symbol ptrs to be used long term (provided they are stored elsewhere).
 *
 * Note: Symbol Table implicitly always has one, un-poppable scope.
 */
class SymbolTable {
public:
	/*
	 * Attempts to create and return a new symbol for the given name.
	 *
	 * Given name is used to dynamically allocate a new symbol paired to that
	 * name in the most local scope. If a symbol with that name already exists
	 * in the local scope, a nullptr is returned. Otherwise, the new symbol is
	 * allocated, paired to the name, and its ptr is returned.
	 *
	 * @param name name to pair with the newly created symbol
	 * @return new symbol paired with name if successful, nullptr otherwise
	 */
	Symbol* addSym(std::string name);

	/*
	 * Pushes/pops one scope from the symbol table.
	 *
	 * Functions allow user to add/remove a scope from the symbol table. Scope
	 * added or removed will always be the most localized scope. Pushing/popping
	 * scopes will affect results of getLocal() and getGlobal().
	 */
	void pushScope(void) {m_table.push_back({});}
	void popScope(void) {if (m_table.size()) {m_table.pop_back();}}

	/*
	 * Various getters for fetching existing name/symbol pairs.
	 *
	 * Getters provide way to get (and check for) various symbols. A local
	 * search will only search the most local scope currently being tracked. A
	 * global search will search all currently tracked scopes, returning the
	 * most local symbol if multiple symbols use the same name.
	 *
	 * @param name name paired with the desired symbol
	 * @return symbol paired with the name if it exists, nullptr otherwise
	 */
	Symbol* getLocal(std::string name);
	Symbol* getGlobal(std::string name);

	/*
	 * Returns the number of scope currently being tracked.
	 *
	 * @return number of scopes being recorded
	 */
	int getScopeNum(void) {return m_table.size();}

private:
	// Vector of maps- each pairing names to symbol pointers.
	std::vector<std::unordered_map<std::string,Symbol*>> m_table;
};

#endif /* SYMBOLTABLE_H_ */
