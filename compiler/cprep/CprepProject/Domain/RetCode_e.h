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
	RET_NOT_CLI = 0x02, // unknown cli argument
	RET_FNO_ARG = 0x03, // cli flag requires argument
	RET_NO_FILE = 0x04, // no input file(s) provided
	RET_BAD_ARG = 0x05  // cli flag with bad argument
} RetCode_e;

#endif /* DOMAIN_RETCODE_E_H_ */
