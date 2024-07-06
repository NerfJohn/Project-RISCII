/*
 * FileReader.h
 *
 * " Device used to read input files byte for byte"
 */

#ifndef SRC_DEVICELAYER_FILEREADER_H_
#define SRC_DEVICELAYER_FILEREADER_H_

#include <string>
#include "DomainLayer/ReadFile_t.h"

/*
 * Device abstracting the access/read process for input files.
 *
 * Files are opened and closed with instances. Instances be checked for being
 * successfully opened and next byte (peeked or popped).
 */
class FileReader {
public:
	/*
	 * Constructs file reader for given filename. File opened on construction.
	 *
	 * Filename is used to open file for reading while constructing the reader.
	 * Construction occurs regardless of opening result, but can be checked with
	 * FileReader.isReady().
	 *
	 * @param filename name of file to read from
	 * @return FileReader instance with already opened (or attempted open) file
	 */
	FileReader(std::string filename);

	/*
	 * Checks if opening file for reading was successful.
	 *
	 * @return true if file was successfully opened, false otherwise
	 */
	bool isReady(void);

	/*
	 * Peeks next available byte in file (if opened successfully).
	 *
	 * Function only defined if FileReader.isReady() returns true. Function
	 * return is undefined otherwise.
	 *
	 * @return next byte in the read file
	 */
	uint8_t peekByte(void);

	/*
	 * Pops next available byte in file (if opened successfully).
	 *
	 * Returns next byte while incrementing read point to next byte in file.
	 * Function only defined if FileReader.isReady() returns true. Function
	 * return is undefined otherwise.
	 *
	 * @return next byte in the read file
	 */
	uint8_t popByte(void);

	/*
	 * Destructs the FileReader instance, closing the file in the process.
	 *
	 * Filename given upon construction is closed (if initial file opening was
	 * successful- otherwise nothing is done).
	 */
	~FileReader(void);

private:
	// Direct file being operated on.
	ReadFile_t m_file;

	// File access status (on opening- trusting opened file stays open).
	bool m_fileOpened;
};

#endif /* SRC_DEVICELAYER_FILEREADER_H_ */
