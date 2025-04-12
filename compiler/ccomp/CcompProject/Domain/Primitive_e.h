/*
 * Primitive_e.h: Enum defining differnt "core" types of data.
 */

#ifndef DOMAIN_PRIMITIVE_E_H_
#define DOMAIN_PRIMITIVE_E_H_

/*
 * @brief Defines "core" types of data. Focused on type checking/comparing.
 *
 * Literal distinguished for type checking reasons. Otherwise maps direct to
 * C-source code.
 */
typedef enum {
	// Real values.
	PRIMITIVE_CHAR,
	PRIMITIVE_INT,
	PRIMITIVE_VOID,
	PRIMITIVE_LIT
} Primitive_e;

#endif /* DOMAIN_PRIMITIVE_E_H_ */
