/*
 * Symbol_t.h: Struct storing data related to lvalue item in program.
 */

#ifndef DOMAIN_SYMBOL_T_H_
#define DOMAIN_SYMBOL_T_H_

#include "Domain/Type_t.h"

/*
 * @brief Info regarding given "lvalue" item in program.
 */
typedef struct {
	// Type-related info.
	Type_t m_type;
} Symbol_t;

#endif /* DOMAIN_SYMBOL_T_H_ */
