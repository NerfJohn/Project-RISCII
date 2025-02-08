/*
 * OsFile.h: Abstraction layer of a (single) file in OS file system.
 */

#ifndef COMMON_OS_OSFILE_H_
#define COMMON_OS_OSFILE_H_

#include <cstdint>
#include <string>
#include "Common/Domain/FileOp_e.h"
#include "Common/Domain/RetErr_e.h"

/*
 * @brief Opens given file for given operation.
 *
 * Attempts to open filename for given operation. Should only be called when
 * previous file was closed (via OsFile_close()) or no files have been opened.
 *
 * @param name path/name of file to open
 * @param dir  operation to begin with file (namely read/write)
 * @return     RET_ERR_NONE on success, RET_ERR_ERROR otherwise
 */
RetErr_e OsFile_open(std::string const& name, FileOp_e const dir);

/*
 * @brief Peeks byte from (open and reading) file.
 *
 * Populates byte with next byte from file (without incrementing file pointer).
 * File must be opened with FILE_OP_READ. Returns EOF (0xFF) if no byte exists.
 *
 * @param byte byte to populate with peek byte
 */
RetErr_e OsFile_peek(uint8_t& byte);

/*
 * @brief Pops byte from (open and reading) file.
 *
 * Populates byte with next byte from file (incrementing file pointer as well).
 * File must be opened with FILE_OP_READ. Returns EOF (0xFF) if no byte exists.
 *
 * @param byte byte to populate with pop byte
 */
RetErr_e OsFile_pop(uint8_t& byte);

/*
 * @brief Appends byte to (open and writing) file.
 *
 * Writes byte to end of file. File must be opened with FILE_OP_WRITE.
 */
RetErr_e OsFile_write(uint8_t const byte);

/*
 * @brief Closes file (if open)- has no effect otherwise.
 */
void OsFile_close(void);

#endif /* COMMON_OS_OSFILE_H_ */
