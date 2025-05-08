/*
 * Type_t.h: Exact 'kind' of a variable or operation.
 */

#ifndef DOMAIN_TYPE_T_H_
#define DOMAIN_TYPE_T_H_

#include <cstdint>
#include "Domain/CoreType_e.h"

/*
 * @brief Specifics of nature of a variable or result of an operation.
 *
 * Intended to work in static and dynamic cases (eg declarations and
 * intermediates).
 */
typedef struct type_t{
	// "Explicit" attributes.
	CoreType_e m_type;   // 'kind' of type
	uint32_t   m_ptrCnt; // 'level' of ptr-ness

	// "Implicit" attributes.
	bool       m_hasLoc; // l-value vs r-value

	/*
	 * @brief Constructor for ensuring common initial values.
	 */
	type_t(void) {
		// Init.
		m_type   = CORE_TYPE_INVALID;
		m_ptrCnt = 0;

		m_hasLoc = false;
	}
} Type_t;

#endif /* DOMAIN_TYPE_T_H_ */
