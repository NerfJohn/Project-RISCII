/*
 * CliFlag_e.h: Flags passed to program via cli command call.
 */

#ifndef DOMAIN_CLIFLAG_E_H_
#define DOMAIN_CLIFLAG_E_H_

/*
 * @brief Enum of cli argument flags program understands.
 *
 * Values split based on flag's expectation of an argument. Special values
 * included to distinguish the split in values and "unknown" value (for use in
 * conversions).
 */
typedef enum {
	// Group #1: Flags with argument.
	CLI_FLAG_DEF,                                // var definition for all files
	CLI_FLAG_INC_DIR,                            // include directory
	CLI_FLAG_NAME,                               // name output file
	CLI_FLAG_LOG_LEVEL,                          // log level to display up to

	// Group #2: Flags without argument.
	CLI_FLAG_HELP,                               // request program help menu
	CLI_FLAG_VERSION,                            // request version info
	CLI_FLAG_TO_CPP,                             // pre-process only
	CLI_FLAG_DEBUG,                              // add debug symbols to image
	CLI_FLAG_O1,                                 // do minor optimizations

	// Special values.
	CLI_FLAG_INVALID,                            // invalid/unknown flag
	CLI_FLAG_ARG_MAX = CLI_FLAG_LOG_LEVEL        // largest Group #1 value
} CliFlag_e;

#endif /* DOMAIN_CLIFLAG_E_H_ */
