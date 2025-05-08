/*
 * Sym_t.h: Symbol representing element of text/data in program.
 */

#ifndef DOMAIN_SYM_T_H_
#define DOMAIN_SYM_T_H_

#include <vector>
#include "Common/Util/Ptr.h"
#include "Domain/Type_t.h"

/*
 * @brief Data regarding a data or code element referenced in a program.
 *
 * Intended to be stored/referenced within a symbol table.
 */
typedef struct sym_t{
	// Primary values.
	Ptr<Type_t> m_type;                   // 'kind' of symbol
	bool        m_isDefined;              //
	std::string m_file;                   // origin file
	uint32_t    m_line;                   // origin line

	// Additional "Func" Info.
	Ptr<Type_t>              m_retType; // 'kind' of return
	std::vector<Ptr<Type_t>> m_params;  // 'kind' of params

	/*
	 * @brief Constructor for ensuring common initial values across use cases.
	 */
	sym_t(void) {
		// Init.
		m_type      = Ptr<Type_t>(nullptr);
		m_isDefined = false;
		m_file      = "";
		m_line      = 0;

		m_retType   = Ptr<Type_t>(nullptr);
		m_params    = {};
	}
} Sym_t;

#endif /* DOMAIN_SYM_T_H_ */
