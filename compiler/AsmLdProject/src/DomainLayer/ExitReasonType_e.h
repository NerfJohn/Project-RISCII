/*
 * ExitReasonType_e.h
 *
 * "Exit codes defining reason program has terminated"
 */

#ifndef SRC_DOMAINLAYER_EXITREASONTYPE_E_H_
#define SRC_DOMAINLAYER_EXITREASONTYPE_E_H_

/*
 * Defines exit codes (and by extension reasons) for terminating program.
 *
 * Each code corresponds to a unique scenario. Having the program return these
 * values gives the calling program info about the run (assuming the codes are
 * called per their scenario properly.
 */
typedef enum {
	// Successful values.
	REASON_SUCCESS = 0,  // successful run

	// Special values.
	REASON_ASSERT  = 255 // design error (oof!)- covers all "unknown" errors
} ExitReasonType_e;

#endif /* SRC_DOMAINLAYER_EXITREASONTYPE_E_H_ */
