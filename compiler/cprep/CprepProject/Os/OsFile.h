/*
 * OsFile.h: Abstraction of a (single) file in OS file system.
 */

#ifndef OS_OSFILE_H_
#define OS_OSFILE_H_

#include <cstdint>
#include <string>
#include "Domain/FileOp_e.h"
#include "Domain/RetErr_e.h"

/*
 * Opens file with the specified path/name and direction.
 *
 * Attempts to open filename for given operation. This function should only be
 * called when the previous file has been closed with OsFile_close() (or it's
 * the first time).
 *
 * @param name path/name of the file to open
 * @param dir  operation to perform on the file (ie read vs write)
 * @return     RET_ERR_NONE on success, RET_ERR_ERROR otherwise
 */
RetErr_e OsFile_open(std::string const& name, FileOp_e const dir);

/*
 * Peeks next char from file (if current file is open and reading).
 *
 * Returns (by reference) the next byte of the current file without incrementing
 * the file pointer. File must be open with FILE_OP_READ to work properly. If no
 * bytes are left, EOF (0xFF) is returned instead.
 *
 * @param byte reference to populate with peeked byte
 * @return     RET_ERR_NONE on success, RET_ERR_ERROR otherwise
 */
RetErr_e OsFile_peek(uint8_t& byte);

/*
 * Pops next char from file (if current file is open and reading).
 *
 * Returns (by reference) the next byte of the current file, incrementing the
 * file pointer in the process. File must be open with FILE_OP_READ to work
 * properly. If no bytes are left, EOF (0xFF) is returned instead.
 *
 * @param byte reference to populate with popped byte
 * @return     RET_ERR_NONE on success, RET_ERR_ERROR otherwise
 */
RetErr_e OsFile_pop(uint8_t& byte);

/*
 * Writes byte to (end of) file (if current file is open and writing).
 *
 * Writes byte to file, incrementing the file (write) pointer in the process.
 * File must be open with FILE_OP_WRITE to work properly.
 *
 * @param byte byte to write to file
 */
RetErr_e OsFile_write(uint8_t const byte);

/*
 * Closes any file currently open. Returns without error regardless of process.
 */
void OsFile_close(void);

#endif /* OS_OSFILE_H_ */
