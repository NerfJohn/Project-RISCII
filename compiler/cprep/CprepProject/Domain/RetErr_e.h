/*
 * RetErr_e.h: General return values for defensive programming techniques.
 */

#ifndef DOMAIN_RETERR_E_H_
#define DOMAIN_RETERR_E_H_

/*
 * Enum defining the result of a given process.
 *
 * This enum is meant to be used for "defensively programmed" functions. The
 * concept dictates that callers need only "if(RetErr_e)" to execute any
 * failure handling code. It can be expanded, though 2 values suffice.
 */
typedef enum {
	// Real values.
	RET_ERR_NONE = 0, // No error occurred
	RET_ERR_ERROR     // an unspecified error occurred
} RetErr_e;

#endif /* DOMAIN_RETERR_E_H_ */
