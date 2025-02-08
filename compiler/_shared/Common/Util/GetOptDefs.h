/*
 * GetOptDefs.h: Macros specialized for usage with Common/Device/GetOpt.h.
 */

#ifndef COMMON_DEVICE_GETOPT_H_
#define COMMON_DEVICE_GETOPT_H_

#include <string>

// Prefix for flag tokens.
#define FLAG_PREFIX ('-')

// Special macros to specialized for "string to flag" function.
#define AS_FLAG(str,type) if (0 == flagStr.compare(std::string("") + \
		                  FLAG_PREFIX + (str))) {return (type);}

#endif /* COMMON_DEVICE_GETOPT_H_ */
