/*
 * ProcDepth_e.h: "Depth" of processing to do on input files.
 */

#ifndef DOMAIN_PROCDEPTH_E_H_
#define DOMAIN_PROCDEPTH_E_H_

/*
 * @brief Defines different processing "depths"/outcomes possible.
 *
 * Effectively determines "output" of program by indicating to skip later steps.
 */
typedef enum {
	// Real "values".
	PROC_TO_CPP,      // pre-process only
	PROC_TO_ASM,      // pre-process + compile (ie don't assemble)
	PROC_TO_BIN       // perform full processing
} ProcDepth_e;

#endif /* DOMAIN_PROCDEPTH_E_H_ */
