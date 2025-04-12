/*
 * Type_t.h: Information describing type of data.
 */

#ifndef DOMAIN_TYPE_T_H_
#define DOMAIN_TYPE_T_H_

#include <cstdint>
#include "Domain/Primitive_e.h"

/*
 * @brief Comprehensive description of variable/literal's type.
 *
 * Intended to be used for type checking. Can be tied to either a variable or
 * literal value.
 */
typedef struct {
	// Core of the type.
	Primitive_e m_type;

	// Keyword modifiers.
	bool        m_isUnsigned;
	bool        m_isVolatile;

	// Additional modifiers.
	std::size_t m_ptrCnt;
	bool        m_isArray;
} Type_t;

#endif /* DOMAIN_TYPE_T_H_ */
