/*
 * OsFile.h
 *
 * "Wrapper for OS specific file interactions"
 */

#ifndef SRC_OSLAYER_OSFILE_H_
#define SRC_OSLAYER_OSFILE_H_

#include <string>
#include "DomainLayer/ReadFile_t.h"

/*
 * Opens specified file for reading (assigned to ReadFile_t).
 *
 * Return value is used to denote success of the operation. the ReadFile_t will
 * not be valid otherwise.
 *
 * @param file file "interface" being accessed for reading
 * @param filename name of actual file to interface to for reading
 * @return true if file was opened for reading, false otherwise
 */
bool OsFile_openReadFile(ReadFile_t* file, std::string filename);

/*
 * Reads next available byte in file, but does not consume it/move on.
 *
 * Return value is used to denote success of the operation. the byte will
 * not be valid otherwise.
 *
 * @param file file "interface" being accessed for reading
 * @param byte byte peeked from file (updated by function)
 * @return true if file's next byte was peeked, false otherwise
 */
bool OsFile_peekByte(ReadFile_t* file, uint8_t* byte);

/*
 * Reads next available byte in file, consuming it/moving on in the process.
 *
 * Return value is used to denote success of the operation. the byte will
 * not be valid otherwise.
 *
 * @param file file "interface" being accessed for reading
 * @param byte byte popped from file (updated by function)
 * @return true if file's next byte was popped, false otherwise
 */
bool OsFile_popByte(ReadFile_t* file, uint8_t* byte);

/*
 * Closes read file (if file passed in was open).
 *
 * If file was already closed (or never opened), this function has no effect.
 *
 * @param file file "interface" being accessed for closing
 */
void OsFile_closeFile(ReadFile_t* file);


#endif /* SRC_OSLAYER_OSFILE_H_ */
