/*
 * OptLevel_e.h: Definitions for levels of optimization available.
 */

#ifndef DOMAIN_OPTLEVEL_E_H_
#define DOMAIN_OPTLEVEL_E_H_

/*
 * @brief Defines levels of optimizations that can be performed
 */
typedef enum {
	// "Real" values.
	OPT_LVL_NONE,    // no optimizations (ie O0)
	OPT_LVL_O1,      // O1 optimizations
	OPT_LVL_O2       // O2 optimizations
} OptLevel_e;

#endif /* DOMAIN_OPTLEVEL_E_H_ */
