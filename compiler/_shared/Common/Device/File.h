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
 * @brief Singleton class providing error-checking logic to file operations.
 *
 * Implements "quality of life" logic for caller- handling redundant error
 * checks and queries. Methods tend to be explicit based on state of class/file
 * (ie open vs closed, operation type).
 */
class File {
public:
	// Enforce File class as a singleton.
	File(File const&)           = delete;
	void operator=(File const&) = delete;

	/*
	 * @brief Retrieves the singleton instance for operations.
	 *
	 * @return reference to File singleton
	 */
	static File& inst(void);

	/*
	 * @brief Returns name of (opened) file.
	 *
	 * Only call when file is open.
	 */
	std::string getName(void);

	/*
	 * @brief Opens given file for given operation.
	 *
	 * Upon success, class now represents an opened file (for the given
	 * operation). Failure indicates class does NOT represent any file. Only
	 * call when file is NOT open.
	 *
	 * @param name name/path of the file
	 * @param dir  operation performed on file
	 * @return     RET_ERR_NONE if successful, RET_ERR_ERROR otherwise
	 */
	RetErr_e open(std::string const& name, FileOp_e const dir);

	/*
	 * @brief Peeks byte from file (open and FILE_OP_READ operation).
	 *
	 * Gets next byte without incrementing file's read pointer. Returns EOF
	 * (0xFF) when no byte exists. Only call when file is open and has been
	 * opened with FILE_OP_READ.
	 *
	 * @return byte peeked from file
	 */
	uint8_t peek(void);

	/*
	 * @brief Pops byte from file (open and FILE_OP_READ operation).
	 *
	 * Gets next byte while also increments file's read pointer. Returns EOF
	 * (0xFF) when no byte exists. Only call when file is open and has been
	 * opened with FILE_OP_READ.
	 *
	 * @return byte popped from file
	 */
	uint8_t pop(void);

	/*
	 * @brief Writes data to file (open and FILE_OP_WRITE operation).
	 *
	 * Appends byte or string to end of file. Only call when file is open and
	 * has been opened with FILE_OP_WRITE.
	 *
	 * @param byte byte to append to file
	 * @param str  string to append to file
	 */
	void write(uint8_t const byte);     // Option 1: byte
	void write(std::string const& str); // Option 2: string

	/*
	 * @brief Closes file (if open). Has no effect on closed file.
	 */
	void close(void);

private:
	// Local metrics for assertions/available info.
	static bool        s_isOpen;
	static std::string s_name;
	static FileOp_e    s_dir;

	// Hide constructor/destructor to force as singleton.
	File()  {/* no actions */}
	~File() {/* no actions */}
};

#endif /* COMMON_DEVICE_FILE_H_ */
