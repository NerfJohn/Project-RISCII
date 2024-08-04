/*
 * FileWriter.cpp
 *
 * "Device used to abstract creation/writing process of a file"
 */

#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "OsLayer/OsFile.h"
#include "Utils/ErrorUtils.h"

#include "DeviceLayer/FileWriter.h"

using namespace std;

//==============================================================================
// Constructs file writer for given filename. File opened on construction.
FileWriter::FileWriter(DataModel_t& model, std::string const filename) {
	// Attempt to open the given filename for reading.
	int retErr = OsFile_openWriteFile(m_file, filename);
	m_isFileOpen = (retErr == 0);

	// Report errors as needed.
	if (!m_isFileOpen) {
		// Log failure to open the file.
		string errStr = string("Failure to create/open file \"")
				        + filename
						+ "\"";
		Printer::getInst()->log(LOG_ERR, errStr);
		ErrorUtils_includeReason(&model, REASON_NO_WRITE);
	}
}

//==============================================================================
// Indicates if file is/was successfully opened.
bool FileWriter::isOpen(void) {
	// Return based on result of opening file during construction.
	return m_isFileOpen;
}

//==============================================================================
// Appends byte to file.
void FileWriter::writeByte(uint8_t const byte) {
	// Attempt to write byte.
	int retErr = OsFile_writeByte(m_file, byte);

	// Failed write (while presumed open)? Assert!
	if (retErr) {
		Printer::getInst()->printAssert("file unavailable for writing");
		Terminate::getInst()->exit(REASON_ASSERT);
	}
}

//==============================================================================
// Destructs the FileWrite instance, closing the file in the process.
FileWriter::~FileWriter(void) {
	// Ensure file is closed before destruction.
	OsFile_closeFile(m_file);
}
