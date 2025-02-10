/*
 * GetOptDefs.h: Macros specialized for usage with Common/Device/GetOpt.h.
 */

#ifndef COMMON_DEVICE_GETOPTDEFS_H_
#define COMMON_DEVICE_GETOPTDEFS_H_

#include <string>

// Prefix for flag tokens.
#define FLAG_PREFIX ('-')

// Macros specialized for "string to 'flag enum'" function.
#define AS_FLAG(str,flg,type) if (0 == str.compare(std::string("") + \
		                      FLAG_PREFIX + (flg))) {return (type);}

#endif /* COMMON_DEVICE_GETOPTDEFS_H_ */
