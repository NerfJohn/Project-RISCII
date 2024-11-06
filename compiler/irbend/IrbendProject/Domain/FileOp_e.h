/*
 * FileOp.h: Specifier for operation being performed on a file.
 */

#ifndef DOMAIN_FILEOP_E_H_
#define DOMAIN_FILEOP_E_H_

/*
 * Enum defining different operations a file can be opened for.
 *
 * In general, a file will only be opened either to be read of written (ie a
 * boolean value). This enum is provided, however, as a means of making the
 * choice/direction of data clearer (ie fancy #define).
 */
typedef enum {
	// Real values.
	FILE_OP_READ,  // reading from file
	FILE_OP_WRITE, // writing to file, clear previous contents
} FileOp_e;

#endif /* DOMAIN_FILEOP_E_H_ */
