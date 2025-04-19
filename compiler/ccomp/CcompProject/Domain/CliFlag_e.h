/*
 * CliFlag_e.h: Flags passed to program via cli command call.
 */

#ifndef DOMAIN_CLIFLAG_E_H_
#define DOMAIN_CLIFLAG_E_H_

#include "Common/Util/GetOptDefs.h"

/*
 * @brief Enum of cli argument flags program understands.
 *
 * Values split based on flag's expectation of an argument. Special values
 * included to distinguish the split in values and "unknown" value (for use in
 * conversions).
 */
typedef enum {
	// Group #1: Flags without argument.
	CLI_FLAG_HELP = GETOPT_BAD_FLAG + 1,         // request program help menu
	CLI_FLAG_VERSION,                            // request version info
	CLI_FLAG_O1,                                 // optimize program

	// Group #2: Flags with argument.
	CLI_FLAG_LOG_LEVEL,                          // log level to display up to

	// Special values.
	CLI_FLAG_INVALID = GETOPT_BAD_FLAG,          // invalid/unknown flag
	CLI_FLAG_ARG_MIN = CLI_FLAG_LOG_LEVEL        // smallest Group #2 value
} CliFlag_e;

#endif /* DOMAIN_CLIFLAG_E_H_ */
