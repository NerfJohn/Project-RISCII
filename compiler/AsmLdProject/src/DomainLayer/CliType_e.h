/*
 * CliType_e.h
 *
 * "General types of inputs available at the CLI level of the program"
 */

#ifndef SRC_DOMAINLAYER_CLITYPE_E_H_
#define SRC_DOMAINLAYER_CLITYPE_E_H_

/*
 * Definitions for general type of inputs given at the command line.
 *
 * Types are used to distinguish pieces of the CLI input as pertinent to
 * processing them into the data model. Further distinction of options are
 * implemented by the user of this enum.
 */
typedef enum {
	CLI_FILE,      // input filename
	CLI_LONE_FLAG, // flag with no arguments
	CLI_ARG_FLAG,  // flag with one following argument
	CLI_BAD_FLAG   // token with flag format, but no matching flag
} CliType_e;

#endif /* SRC_DOMAINLAYER_CLITYPE_E_H_ */
