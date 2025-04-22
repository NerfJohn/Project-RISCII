/*
 * GetOptDefs.h: Macros specialized for usage with Common/Device/GetOpt.h.
 */

#ifndef COMMON_DEVICE_GETOPTDEFS_H_
#define COMMON_DEVICE_GETOPTDEFS_H_

#include <string>

/*
 * @brief Definitions to meet GetOpt's needs in a streamlined fashion.
 *
 * GetOpt relies on two user-defined functions: asFlag() and needArg(). asFlag()
 * identifies flags defined by the user program. needArg() indicates if a
 * user defined flag requires an argument.
 *
 * Function signatures for asFlag() and needArg() are provided. Copying these
 * signatures will ensure the following macros work as intended.
 *
 * The definition GETOPT_BAD_FLAG is used to indicate an invalid flag was
 * detected. The user program should avoid using this value for a valid flag.
 *
 * The macro AS_FLAG() is intended to be used within asFlag() as a quick means
 * of performing the conversion (provided the given func decl is used below).
 *
 * The recommended approach is to create an enum of all flags. Set the error
 * value to GETOPT_BAD_FLAG and group 'lone' and 'arg' flags into two groups-
 * separated and distinguished by sentinel values.
 */

//=== "Internal Values" ========================================================

// Prefix for all flag tokens.
#define FLAG_PREFIX ('-')

//=== "External Values" ========================================================

// User-specific functions GetOpt relies on for parsing.
int asFlag(std::string str);
bool needArg(int flgCode);

// Definitions used by GetOpt for user specific values.
#define GETOPT_BAD_FLAG -1

// Macros specialized for "asFlag" function.
#define AS_FLAG(flg, type) if (str.compare(std::string("") + FLAG_PREFIX + \
		                   (flg)) == 0) {return (type);}

#endif /* COMMON_DEVICE_GETOPTDEFS_H_ */
