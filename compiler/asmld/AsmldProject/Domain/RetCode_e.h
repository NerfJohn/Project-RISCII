/*
 * RetCode_e.h: Various return codes program can return upon exiting.
 */

#ifndef DOMAIN_RETCODE_E_H_
#define DOMAIN_RETCODE_E_H_

/*
 * Enum defining possible outputs of the program when it returns.
 *
 * These values provide a way to determine the result of calling the program
 * without having to strictly rely on serial/file outputs. Each code should
 * represent a unique, program-ending scenario.
 */
typedef enum {
	// Real values.
	RET_SUCCESS = 0x00, // Successful run of program
	RET_ASSERT  = 0x01, // assertion failed (ie program bug)
	RET_NO_READ = 0x02, // unable to open file for reading
	RET_BAD_LEX = 0x03, // unrecognized character sequence
	RET_MIS_TKN = 0x04, // mismatched token (parsing)
	RET_UEX_TKN = 0x05, // unexpected token (parsing)
	RET_BAD_FLG = 0x06, // invalid flag
	RET_BAD_REG = 0x07, // invalid register
	RET_BAD_IMM = 0x08, // invalid immediate
	RET_BIG_BIN = 0x09, // image is too large
	RET_BIG_RAM = 0x0A, // ram use is too large
	RET_NO_WRIT = 0x0B, // unable to open file for writing
	RET_L_REDEF = 0x0C, // multiply defined label- local
	RET_NO_PAIR = 0x0D, // label not paired to instruction/data
	RET_L_NODEF = 0x0E, // label not defined locally
	RET_G_REDEF = 0x0F  // multiply defined label- global
} RetCode_e;

#endif /* DOMAIN_RETCODE_E_H_ */
