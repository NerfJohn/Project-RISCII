/*
 * ProcDepth_e.h: TODO
 */

#ifndef DOMAIN_PROCDEPTH_E_H_
#define DOMAIN_PROCDEPTH_E_H_

// TODO
typedef enum {
	// Real "values".
	PROC_TO_CPP,      // pre-process only
	PROC_TO_ASM,      // pre-process + compile (ie don't assemble)
	PROC_TO_BIN       // perform full processing
} ProcDepth_e;

#endif /* DOMAIN_PROCDEPTH_E_H_ */
