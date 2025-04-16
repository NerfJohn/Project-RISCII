/*
 * RetErr_e.h: Type specifying defensive programming return values.
 */

#ifndef COMMON_DOMAIN_RETERR_E_H_
#define COMMON_DOMAIN_RETERR_E_H_

/*
 * @brief Enum of defensive programming return values.
 *
 * Values defined with "if(isErr)" in mind. Meant to allow for quick/convenient
 * error checking when using defensive programming techniques.
 */
typedef enum {
	// Real values.
	RET_ERR_NONE  = 0, // No error occurred
	RET_ERR_ERROR = 1  // an (unspecified) error occurred
} RetErr_e;

#endif /* COMMON_DOMAIN_RETERR_E_H_ */
