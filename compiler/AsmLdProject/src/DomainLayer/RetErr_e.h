/*
 * RetErr_e.h
 *
 * "Generic return codes to indicate errors/error paths"
 */

#ifndef SRC_DOMAINLAYER_RETERR_E_H_
#define SRC_DOMAINLAYER_RETERR_E_H_

/*
 * Enum defining generic return codes to indicating function success.
 *
 * Codes are intentionally generic to allow use for any/every function that
 * does not require specific return codes. Explicit values are assigned
 * assuming callers will check for errors like so: "if(RetErr_e)".
 */
typedef enum {
	RET_GOOD = 0, // explicitly zero to NOT pass if(RetErr_e) checks
	RET_FAIL = 1  // explicitly non0zero to trigger if(RetErr_e) checks
} RetErr_e;

#endif /* SRC_DOMAINLAYER_RETERR_E_H_ */
