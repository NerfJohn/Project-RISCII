/*
 * CliType_e.h: Types for command line inputs to a program.
 */

#ifndef DOMAIN_CLITYPE_E_H_
#define DOMAIN_CLITYPE_E_H_

/*
 * @brief Enum of different outcomes from parsing cli arguments.
 *
 * Divided into 2 groups: 1) flag with argument 2) everything else. Sentinel
 * value provided to ease distinguishment of "arg flags" by enum ID's value.
 */
typedef enum {
	// "Flag with Argument" values (ie Group #1).

	// TODO- add IDs for flags with args.
	// TODO- set CLI_FLAG_ARG_MAX to highest ID in this group.

	// Other real values (ie Group #2).
	CLI_FILE,                          // path to file (ie non flag)
	CLI_INVALID,                       // unrecognized cli argument
	CLI_NO_ARG,                        // "arg flag" with no argument

	// TODO- add IDs for flags without args.

	// "Sentinel" values.
	CLI_FLAG_ARG_MAX  = // TODO        // highest value "arg flag" ID
} CliType_e;

#endif /* DOMAIN_CLITYPE_E_H_ */
