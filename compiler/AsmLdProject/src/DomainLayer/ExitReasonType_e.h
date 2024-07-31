/*
 * ExitReasonType_e.h
 *
 * "Exit codes defining reason program has terminated"
 */

#ifndef SRC_DOMAINLAYER_EXITREASONTYPE_E_H_
#define SRC_DOMAINLAYER_EXITREASONTYPE_E_H_

/*
 * Defines exit codes (and by extension reasons) for terminating program.
 *
 * Each code corresponds to a unique scenario. Having the program return these
 * values gives the calling program info about the run (assuming the codes are
 * called per their scenario properly.
 */
typedef enum {
	// Special values (0x00 - 0x3F).
	REASON_SUCCESS     = 0x00, // Successful build/assembly
	REASON_ASSERT      = 0x01, // Assert/Unknown error occurred

	// Warning values (0x40 - 0x7F).
	REASON_WARNING_MIN = 0x40, // Sentinel value for warning range
	REASON_REPEAT_FLAG = 0x40, // Repeated flag in flag token
	REASON_NOUSE_LABEL = 0x41, // Label is defined, but unused
	REASON_WARNING_MAX = 0x7F, // Sentinel value for warning range

	// Error values (0x80 - 0xBF).
	REASON_ERROR_MIN    = 0x80, // Sentinel value for error range
	REASON_NO_FILE      = 0x80, // Could not file or open file
	REASON_BAD_SCAN     = 0x81, // Byte sequence wasn't a known token
	REASON_EXP_PARSE    = 0x82, // Expected token, received different token
	REASON_UNEXP_PARSE  = 0x83, // Unexpected token during parsing
	REASON_BAD_FLAG     = 0x84, // Flag not supported by instruction
	REASON_BAD_REG      = 0x85, // Register value was bad/out of bounds
	REASON_BAD_IMM      = 0x86, // Immediate value was bad/out of bounds
	REASON_BIG_TEXT     = 0x87, // Text section is too big for binary
	REASON_BIG_BIN      = 0x88, // Binary image is too big for target
	REASON_NO_WRITE     = 0x89, // Couldn't create/open write file
	REASON_NO_INPUTS    = 0x8A, // No input files provided
	REASON_FLAG_ARG     = 0x8B, // Argument for CLI flag is invalid
	REASON_INVALID_FLAG = 0x8C, // CLI flag is not recognized
	REASON_NO_FLAG_ARG  = 0x8D, // CLI flag requires arg- none given
	REASON_MULTI_DEFINE = 0x8E, // Label is multiply defined
	REASON_NO_PAIRING   = 0x8F, // Label has nothing to pair/address to
	REASON_NO_DEF_LABEL = 0x90, // Label is used, but not defined
	REASON_BIG_DATABSS  = 0x91, // Data + Bss cannot fit in RAM
	REASON_BAD_START    = 0x92, // Start label not tied to text section
	REASON_ERROR_MAX    = 0xBF  // Sentinel value for error range
} ExitReasonType_e;

#endif /* SRC_DOMAINLAYER_EXITREASONTYPE_E_H_ */
