/*
 * SymTable.h: Standard implementation of scoped name/symbol pairs.
 */

#ifndef COMMON_DEVICE_SYMTABLE_H_
#define COMMON_DEVICE_SYMTABLE_H_

#include <map>
#include <vector>
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Domain/RetErr_e.h"

/*
 * @brief Multi-scope table used to record and track symbols across scopes.
 *
 * Symbols are only ever added. Separate local/global function provided. Symbol
 * type must be provided.
 */
template<typename T> class SymTable {
public:
	/*
	 * @brief Adds new scope to symbol table. Scope becomes new "most local".
	 */
	void scopePush(void) {
		// Add.
		m_table.push_back({});
	}

	/*
	 * @brief Removes "most local" scope from table.
	 *
	 * Does NOT clear or take ownership of pointers/symbols. Asserts if called
	 * with 0 scopes.
	 */
	void scopePop(void) {
		// Validate.
		if (0 == m_table.size()) {
			Print_bug("scopePop() no scope");
			Terminate_bug(); // does NOT return
		}

		// Pop.
		m_table.pop_back();
	}

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
	RetErr_e addLocal(std::string const& name, T& symbol) {
		// Result.
		RetErr_e retErr = RET_ERR_ERROR;

		// Get Table Index.
		int idx = m_table.size() - 1;
		if (idx < 0) {
			Print_bug("addLocal() no scope");
			Terminate_bug(); // does NOT return
		}

		// Add Symbol.
		if (m_table[idx].find(name) == m_table[idx].end()) {
			m_table[idx].insert({name, symbol});
			retErr = RET_ERR_NONE;
		}

		// Return.
		return retErr;
	}

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
	RetErr_e addGlobal(std::string const& name, T& symbol) {
		// Result.
		RetErr_e retErr = RET_ERR_ERROR;

		// Validate.
		if (m_table.size() > 0) {
			Print_bug("addGlobal() no scope");
			Terminate_bug(); // does NOT return
		}

		// Add Symbol.
		if (m_table[0].find(name) == m_table[0].end()) {
			m_table[0].insert({name, symbol});
			retErr = RET_ERR_NONE;
		}

		// Return.
		return retErr;
	}

	/*
	 * @brief Get symbol (pointer) associated with key. Biased towards local.
	 *
	 * Searches scopes in order of most local to least. Fails if key is not
	 * found anywhere.
	 *
	 * @param name   key to search for
	 * @param symbol symbol to populate if key is found
	 * @return       RET_ERR_NONE is successful, RET_ERR_ERROR otherwise
	 */
	RetErr_e get(std::string const& name, T& symbol) {
		// Result.
		RetErr_e retErr = RET_ERR_ERROR;

		// Search.
		for (int i = m_table.size() - 1; i >= 0; i--) {
			// Break on Found.
			if (m_table[i].find(name) != m_table[i].end()) {
				symbol = m_table[i].at(name);
				retErr = RET_ERR_NONE;
				break;
			}
		}

		// Return.
		return retErr;
	}

	/*
	 * @brief Gets the total number of entries across all scopes.
	 *
	 * @return total number of symbols across all scopes.
	 */
	size_t size(void) {
		// Sum of Scopes.
		size_t retSize = 0;
		for (std::map<std::string, T> scope : m_table) {
			retSize += scope.size();
		}

		// Voila! the size.
		return retSize;
	}

	/*
	 * @brief Gets total number of scopes current present.
	 *
	 * @return total number of scopes present in the table
	 */
	size_t scopeNum(void) {
		return m_table.size();
	}

private:
	// Internal structure of symbol table.
	std::vector<std::map<std::string, T>> m_table;
};

#endif /* COMMON_DEVICE_SYMTABLE_H_ */
