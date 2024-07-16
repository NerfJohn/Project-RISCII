/*
 * FileWriter.h
 *
 * "Device used to abstract creation/writing process of a file"
 */

#ifndef SRC_DEVICELAYER_FILEWRITER_H_
#define SRC_DEVICELAYER_FILEWRITER_H_

#include <string>
#include "DomainLayer/DataModel_t.h"
#include "DomainLayer/WriteFile_t.h"

/*
 * Device abstracting the access/write process for output files.
 *
 * Files are opened and closed with instances. Class takes care of logging and
 * asserting, but requires user to check FileWriter.isOpen() prior to proper
 * use (ie offensive programmed).
 */
class FileWriter {
public:
	/*
	 * Constructs file writer for given filename. File opened on construction.
	 *
	 * Filename is used to open file for writing while constructing the writer.
	 * Construction occurs regardless of opening result, but can be checked with
	 * FileWriter.isOpen(). Logs own errors (without print/termination).
	 *
	 * @param filename name of file to write from
	 * @param model data model of the program
	 * @return FileWriter instance with already opened (or attempted open) file
	 */
	FileWriter(DataModel_t& model, std::string const filename);

	/*
	 * Indicates if file is/was successfully opened.
	 *
	 * Returns result from trying to open the file on instance construction.
	 *
	 * @return true is file is open for writing, false otherwise
	 */
	bool isOpen(void);

	/*
	 * Appends byte to file.
	 *
	 * Writes given byte to end of the file. Since the file's write pointer
	 * begins at the start of the file upon opening, this appending naturally
	 * overwrites the old file contents (ie old contents are fully erased).
	 *
	 * @param byte byte value to write to file
	 */
	void writeByte(uint8_t const byte);

	/*
	 * Destructs the FileWrite instance, closing the file in the process.
	 *
	 * Filename given upon construction is closed (if initial file opening was
	 * successful- otherwise nothing is done).
	 */
	~FileWriter(void);

private:
	// Direct file being operated on.
	WriteFile_t m_file;

	// File data gathered at construction.
	bool        m_isFileOpen;
};

#endif /* SRC_DEVICELAYER_FILEWRITER_H_ */
