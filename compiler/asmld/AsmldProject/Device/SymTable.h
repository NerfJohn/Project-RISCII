/*
 * SymTable.h: Table for declaring/modifying/referencing label symbols.
 */

#ifndef DEVICE_SYMTABLE_H_
#define DEVICE_SYMTABLE_H_

#include <map>
#include <vector>
#include "Domain/RetErr_e.h"
#include "Domain/Symbol_t.h"

// Name of required, pre-known label- managed by SymTable.
#define SYM_START_NAME (std::string("__START"))

/*
 * Table used to add/lookup/store program's symbols.
 *
 * Effectively just a storage class for symbols. Can be used to store local
 * and global labels. Tables act as individual instances, but shares common
 * definitions for pre-known labels (e.g. __SIZE, __START) baked into class.
 */
class SymTable {
public:
	/*
	 * Adds given symbol to table (if key has no assigned symbol).
	 *
	 * Addition carried out ONLY IF key doesn't already correspond to a
	 * symbol. Symbol ptr MUST not be NULL. Function factors for pre-known
	 * labels (e.g. __SIZE, __START).
	 *
	 * @param key string used to find symbol later
	 * @param sym symbol to add to table under key
	 * @return    RET_ERR_NONE if successfully added, RET_ERR_ERROR otherwise
	 */
	RetErr_e addSym(std::string const& key, Symbol_t* sym);

	/*
	 * Finds symbol based on key.
	 *
	 * Populates symbol reference ONLY IF symbol is found with the key. Symbol
	 * ptr is guaranteed to NOT be NULL. Function factors for pre-known labels
	 * (e.g. __SIZE, __START).
	 *
	 * @param key key to use to find symbol
	 * @param sym symbol found using the key
	 * @return    RET_ERR_NONE if symbol is found, RET_ERR_ERROR otherwise
	 */
	RetErr_e getSym(std::string const& key, Symbol_t*& sym);

	/*
	 * Populates the given vector with table's symbols.
	 *
	 * Overwrites vector's contents with symbols currently in the symbol table.
	 * Symbol ptrs are guaranteed to NOT be NULL. Pre-known labels (e.g. __SIZE,
	 * __START) are NOT included in the list.
	 *
	 * @param syms vector to overwrite/populate with the table's symbols
	 */
	void asList(std::vector<Symbol_t*>& syms);

private:
	// Core "name to symbol" map of the table.
	std::map<std::string, Symbol_t*> m_table;
};

#endif /* DEVICE_SYMTABLE_H_ */
