/*
 * CliType_e.h: Types of tokens found on command line call of program.
 */

#ifndef COMMON_DOMAIN_CLITYPE_E_H_
#define COMMON_DOMAIN_CLITYPE_E_H_

/*
 * @brief Enum of token types parsed from command line call.
 * 
 * Intended to be used/output by Common/Device/GetOpt.h as a means of inferring
 * the token's general type (vs specific meaning).
 */
typedef enum {
	// Real values.
	CLI_TYPE_FILE,     // file/typical input
	CLI_TYPE_FLAG,     // flag (with or without argument)

	// Special values.
	CLI_TYPE_UNKNOWN,  // unrecognized argument (or unknown flag)
	CLI_TYPE_NO_ARG,   // no argument for flag requiring one
	CLI_TYPE_INVALID   // invalid type (eg no actual type)
} CliType_e;

#endif /* COMMON_DOMAIN_CLITYPE_E_H_ */
