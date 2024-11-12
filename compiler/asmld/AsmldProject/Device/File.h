/*
 * File.h: Standard portal for accessing files (albeit one at a time).
 */

#ifndef DEVICE_FILE_H_
#define DEVICE_FILE_H_

#include <cstdint>
#include <string>
#include "Domain/FileOp_e.h"
#include "Domain/RetErr_e.h"

/*
 * Singleton class for providing access to files (one at a time).
 *
 * Class effectively acts like a "safety wrapper" for OsFile functions. It
 * handles any assertions/error checking on behalf of the caller (besides
 * handling "unable to open file" errors- which are deemed a user issue.
 */
class File {
public:
	// Enforce File class as a singleton.
	File(File const&)           = delete;
	void operator=(File const&) = delete;

	/*
	 * Retrieves the singleton instance for operations.
	 *
	 * @return reference to File singleton
	 */
	static File& inst(void);

	/*
	 * Retrieves name of currently opened file. Should not be called otherwise.
	 *
	 * @return name of currently opened file
	 */
	std::string getName(void);

	/*
	 * Opens file for given operation. Returns based on success of operation.
	 *
	 * Uses name (or path/name) to open the file. If successful, the open file's
	 * info is saved to this singleton, allowing other functions to be called.
	 * The return value indicates if the file was found/opened successfully.
	 *
	 * @param name name/path of the file to open
	 * @param dir  operation to perform on file
	 * @return     RET_ERR_NONE is successful, RET_ERR_ERROR otherwise
	 */
	RetErr_e open(std::string const name, FileOp_e const dir);

	/*
	 * Peeks byte from file (if open and reading).
	 *
	 * Reads byte from file without "consuming" it (from a reading perspective).
	 * File must be opened with FILE_OP_READ in order to be used.
	 *
	 * @return byte peeked from the file
	 */
	uint8_t peek(void);

	/*
	 * Pops byte from file (if open and reading).
	 *
	 * Reads byte from file while "consuming" it (from a reading perspective).
	 * File must be opened with FILE_OP_READ in order to be used.
	 *
	 * @return byte popped from the file
	 */
	uint8_t pop(void);

	/*
	 * Writes byte to file (if open and writing).
	 *
	 * Writes byte to end of file. File must be opened with FILE_OP_WRITE in
	 * order to be used.
	 *
	 * @param byte byte to write to file
	 */
	void write(uint8_t byte);

	/*
	 * Closes any open file.
	 */
	void close(void);

private:
	// Local metrics for assertions/available info.
	static bool        m_isOpen;
	static std::string m_name;
	static FileOp_e    m_dir;

	// Hide constructor/destructor to force as singleton.
	File()  {/* no actions */}
	~File() {/* no actions */}
};

#endif /* DEVICE_FILE_H_ */
