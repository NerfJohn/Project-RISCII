/*
 * FileOp_e.h: Type specifying operation being performed on a file.
 */

#ifndef DOMAIN_FILEOP_E_H_
#define DOMAIN_FILEOP_E_H_

/*
 * @brief Enum of operations that can be performed on a file.
 *
 * Specific operations for files. Intended to be used with Common/Os/OsFile.
 */
typedef enum {
	// Real values.
	FILE_OP_READ,  // read from file
	FILE_OP_WRITE, // write to file (clear previous contents)
} FileOp_e;

#endif /* DOMAIN_FILEOP_E_H_ */
