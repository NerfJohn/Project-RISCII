/*
 * CoreType_e.h: Core primitive used to represent binary data.
 */

#ifndef DOMAIN_CORETYPE_E_H_
#define DOMAIN_CORETYPE_E_H_

/*
 * @brief Enum used to define 'kind' of a specific type or symbol.
 *
 * Invalid option provided to detect lack of provided core type in parsers.
 */
typedef enum {
	// *Core* values (har har).
	CORE_TYPE_INT,
	CORE_TYPE_VOID,

	// Special values.
	CORE_TYPE_INVALID
} CoreType_e;

#endif /* DOMAIN_CORETYPE_E_H_ */
