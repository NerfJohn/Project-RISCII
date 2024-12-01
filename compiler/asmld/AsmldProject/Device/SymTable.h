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
 * can create symbols for the caller, add pre-created entries, and retrieve
 * requested symbols.
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

	/*
	 * Adds the given symbol to the table (if the nameis not already taken).
	 *
	 * If the given name is not already used to access a symbol, the given
	 * symbol will be added under the given name. Nullptrs result in assertion,
	 * with the return indicating the success of the "add" operation.
	 *
	 * @param name name to save the symbol under
	 * @param sym  symbol (ptr) to save to the table
	 * @return     RET_ERR_NONE if added, RET_ERR_ERROR otherwise
	 */
	RetErr_e addSym(std::string const& name, Symbol_t* sym);

	/*
	 * Gets a symbol based on the given name.
	 *
	 * Uses given name to find and return the symbol. Return indicates the
	 * success of the "get" operation.
	 *
	 * @param name name symbol is saved under
	 * @param sym  symbol accessed using the name
	 * @return     RET_ERR_NONE if found, RET_ERR_ERROR otherwise
	 */
	RetErr_e getSym(std::string const& name, Symbol_t*& sym);

private:
	// Core "name to symbol" map of the table.
	std::map<std::string, Symbol_t*> m_table;
};

#endif /* DEVICE_SYMTABLE_H_ */
