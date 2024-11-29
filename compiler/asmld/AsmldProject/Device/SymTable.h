/*
 * SymTable.h: Table for declaring/modifying/referencing label symbols.
 */

#ifndef DEVICE_SYMTABLE_H_
#define DEVICE_SYMTABLE_H_

#include <map>
#include <vector>
#include "Domain/RetErr_e.h"
#include "Domain/Symbol_t.h"
#include "Ds/ItemToken.h"

/*
 * Class used to manage the creation and modification of symbols.
 *
 * Implements logic to create, get, and modify symbols based on labels. Table
 * can create symbols for the caller, though generally expects the caller to
 * "own" the symbol (on creation).
 */
class SymTable {
public:
	/*
	 * Creates a new symbol with a label- returning a link to symbol info.
	 *
	 * Allocates new symbol based on given label's info. Symbol link is returned
	 * for future referencing/reading. If a symbol with the given label name
	 * already exists, it is returned instead.
	 *
	 * @param labelTkn label item new symbol is based on
	 * @param sym      link to symbol associated with the label's name
	 * @return         RET_ERR_NONE if new symbol made, RET_ERR_ERROR otherwise
	 */
	RetErr_e define(ItemToken& labelTkn, Symbol_t*& sym);

private:
	// Core "name to symbol" map of the table.
	std::map<std::string, Symbol_t*> m_table;
};

#endif /* DEVICE_SYMTABLE_H_ */
