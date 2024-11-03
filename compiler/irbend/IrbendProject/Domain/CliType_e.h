/*
 * CliType_e.h: Types of arguments found on command line call of irbend.exe.
 */

#ifndef DOMAIN_CLITYPE_E_H_
#define DOMAIN_CLITYPE_E_H_

/*
 * Enum defining different kinds of tokens used to call irbend.exe.
 *
 * These types are most applicable when parsing the program's command arguments.
 * A special "invalid" value is provided for initial "no (valid) value"
 * scenarios.
 */
typedef enum {
	// Real values.
	CLI_TYPE_FILE,
	CLI_TYPE_LONE,
	CLI_TYPE_ARG,

	// Special values.
	CLI_TYPE_INVALID
} CliType_e;

#endif /* DOMAIN_CLITYPE_E_H_ */
