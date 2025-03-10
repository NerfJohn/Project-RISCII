/*
 * SymTable.h: Standard implementation of scoped name/symbol pairs.
 */

#ifndef COMMON_DEVICE_SYMTABLE_H_
#define COMMON_DEVICE_SYMTABLE_H_

#include <map>
#include <vector>
#include "Common/Domain/RetErr_e.h"

/*
 * @brief Basic Multi-scope Symbol Table.
 *
 * Uses void pointers to allow user to define symbols. Requires non-null
 * symbols to denote failure in get() method.
 */
class SymTable {
public:
	/*
	 * @brief Adds new scope to symbol table. Scope becomes new "most local".
	 */
	void scopePush(void);
	
	/*
	 * @brief Removes "most local" scope from table.
	 *
	 * Does NOT clear or take ownership of pointers/symbols. Asserts if called
	 * with 0 scopes.
	 */
	void scopePop(void);
	
	/*
	 * @brief Adds symbol (tied to key) in "most local" scope.
	 *
	 * Table must have at least 1 scope. Symbol pointer must be non-null. Fails
	 * if key already taken/exists in most local scope.
	 *
	 * @param name   key to associate symbol with
	 * @param symbol symbol to save under key
	 * @return       RET_ERR_NONE if successful, RET_ERR_ERROR otherwise
	 */
	RetErr_e addLocal(std::string const& name, void* symbol);
	
	/*
	 * @brief Adds symbol (tied to key) in "least local" scope.
	 *
	 * Table must have at least 1 scope. Symbol pointer must be non-null. Fails
	 * if key already taken/exists in least local scope.
	 *
	 * @param name   key to associate symbol with
	 * @param symbol symbol to save under key
	 * @return       RET_ERR_NONE if successful, RET_ERR_ERROR otherwise
	 */
	RetErr_e addGlobal(std::string const& name, void* symbol);
	
	/*
	 * @brief Get symbol (pointer) associated with key. Biased towards local.
	 *
	 * Searches scopes in order of most local to least. Returns nullptr if no
	 * symbols are found.
	 *
	 * @param name key to search for
	 * @param      symbol pointer if successful, nullptr otherwise
	 */
	void* get(std::string const& name);

	// TODO
	size_t size(void);

private:
	// Internal structure of symbol table.
	std::vector<std::map<std::string, void*>> m_table;
};

#endif /* COMMON_DEVICE_SYMTABLE_H_ */
