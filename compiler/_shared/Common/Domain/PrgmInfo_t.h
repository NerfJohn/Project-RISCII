/*
 * PrgmInfo_t.h: Structure storing summary info of run instance of program.
 */

#ifndef COMMON_DOMAIN_PRGMINFO_H_
#define COMMON_DOMAIN_PRGMINFO_H_

#include <cstdint>

/*
 * @brief Stores general "run instance" data for a program.
 * 
 * Intended to be used in conjunction with Terminate::inst().summary(). Can be
 * used within data model, or populated by model just before termination usage.
 */
typedef struct {
	// Counters for undesired behavior.
	uint32_t m_numWarns;
	uint32_t m_numErrs;
	
	// Return code to be used by the program.
	int      m_retCode;
} PrgmInfo_t;

#endif /* COMMON_DOMAIN_PRGMINFO_H_ */
