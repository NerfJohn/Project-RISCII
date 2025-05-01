/*
 * RetCode_e.h: Various return codes program can return upon exiting.
 */

#ifndef DOMAIN_RETCODE_E_H_
#define DOMAIN_RETCODE_E_H_

/*
 * @brief Enum defining possible outputs of the program when it returns.
 *
 * These values provide a way to determine the result of calling the program
 * without having to strictly rely on serial/file outputs. Each code should
 * represent a unique, program-ending scenario.
 */
typedef enum {
	// Real values.
	RET_SUCCESS = 0x00, // successful run
	RET_ASSERT  = 0x01, // bug in source code
	RET_NOT_CLI = 0x02, // unknown cli flag/argument
	RET_FNO_ARG = 0x03, // cli flag without required argument
	RET_BAD_ARG = 0x04, // bad cli flag argument
	RET_NO_FILE = 0x05, // no input file(s) given
	RET_NO_READ = 0x06, // unable to read input file
	RET_BAD_LEX = 0x07, // bad lex path/matching
	RET_MIS_TKN = 0x08, // mismatch token (parsing)
	RET_UEX_TKN = 0x09  // unexpected token (parsing)
} RetCode_e;

#endif /* DOMAIN_RETCODE_E_H_ */
