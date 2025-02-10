/*
 * OptInit_t.h: Structure storing init data for GetOpt instance.
 */

#ifndef COMMON_DOMAIN_OPTINIT_T_H_
#define COMMON_DOMAIN_OPTINIT_T_H_

#include <string>

/*
 * @brief Stores data required to initialize a GetOpt instance.
 * 
 * Intended to be used with Common/Device/GetOpt.h. Should be filled in using
 * program's passed in argument and program-tailored handler functions. See
 * Common/Util/GetOptDefs.h for help making those functions.
 */
typedef struct {
	// Arguments from command line.
	int                m_argc;   // number of arguments (in argv)
	char const* const* m_argv;   // ordered array of arguments
	
	// Callback functions for program specific details.
	int  (*m_asFlag)(std::string);    // converts string to 'flag enum'
	bool (*m_needArg)(int flagCode);  // checks arg requirement for 'flag enum'

	// Specific values from 'flag enum' necessary for parsing.
	int m_badFlag; // value associated with invalid flag
} OptInit_t;

#endif /* COMMON_DOMAIN_OPTINIT_T_H_ */
