/*
 * File.h: Standardizes file operations (albeit one file at a time).
 */

#ifndef COMMON_DEVICE_FILE_H_
#define COMMON_DEVICE_FILE_H_

#include <cstdint>
#include <string>
#include "Common/Domain/FileOp_e.h"
#include "Common/Domain/RetErr_e.h"

/*
 * @brief Returns name of (opened) file. Call ONLY when file is open.
 *
 * @return name of opened file
 */
std::string& File_getName(void);

/*
 * @brief Opens given file for given operation.
 *
 * Upon success, File now represents an opened file (for the given
 * operation). Failure indicates class does NOT represent any file. Only
 * call when file is NOT open.
 *
 * @param name name/path of the file
 * @param dir  operation performed on file
 * @return     RET_ERR_NONE if successful, RET_ERR_ERROR otherwise
 */
RetErr_e File_open(std::string const& name, FileOp_e const dir);

/*
 * @brief Peeks byte from file (open and FILE_OP_READ operation).
 *
 * Gets next byte without incrementing file's read pointer. Returns EOF
 * (0xFF) when no byte exists. Only call when file is open and has been
 * opened with FILE_OP_READ.
 *
 * @return byte peeked from file
 */
uint8_t File_peek(void);

/*
 * @brief Pops byte from file (open and FILE_OP_READ operation).
 *
 * Gets next byte while also increments file's read pointer. Returns EOF
 * (0xFF) when no byte exists. Only call when file is open and has been
 * opened with FILE_OP_READ.
 *
 * @return byte popped from file
 */
uint8_t File_pop(void);

/*
 * @brief Writes data to file (open and FILE_OP_WRITE operation).
 *
 * Appends byte or string to end of file. Only call when file is open and
 * has been opened with FILE_OP_WRITE.
 *
 * @param byte byte to append to file
 * @param str  string to append to file
 */
void File_write(uint8_t     const  byte); // Option #1: byte
void File_write(std::string const& str);  // Option #2: string

/*
 * @brief Closes file (if open). Has no effect on closed file.
 */
void File_close(void);

#endif /* COMMON_DEVICE_FILE_H_ */
