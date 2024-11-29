/*
 * Symbol_t.h: Symbol with info regarding a given label/address.
 */

#ifndef DOMAIN_SYMBOL_T_H_
#define DOMAIN_SYMBOL_T_H_

#include <cstdint>
#include <string>
#include "Domain/AddrSpace_e.h"

/*
 * Symbol containing data related to a declared address/label.
 *
 * Contains information used to implement named memory addresses. Acts as a
 * common repository between label definitions and related references. It is
 * intended to be created/managed by a table when linking related nodes.
 */
typedef struct {
	// (Mutable) Name of the symbol.
	std::string m_name;

	// "Location" of symbol's declaration.
	std::string m_file;
	uint32_t    m_line;

	// Address (space) symbol corresponds to.
	AddrSpace_e m_space;
	uint16_t    m_addr;
} Symbol_t;

#endif /* DOMAIN_SYMBOL_T_H_ */
