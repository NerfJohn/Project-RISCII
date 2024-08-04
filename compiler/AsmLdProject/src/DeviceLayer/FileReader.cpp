/*
 * FileReader.cpp
 *
 * " Device used to read input files byte for byte"
 */

#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "OsLayer/OsFile.h"
#include "Utils/ErrorUtils.h"

#include "DeviceLayer/FileReader.h"

using namespace std;

//==============================================================================
// Constructs file reader for given filename. File opened on construction.
FileReader::FileReader(DataModel_t* model, std::string filename) {
	// Save filename (for posterity).
	m_filename = filename;

	// Attempt to open the given filename for reading.
	m_fileOpened = OsFile_openReadFile(&m_file, filename);

	// Report errors as needed.
	if (!m_fileOpened) {
		// Log failure to open the file.
		string errStr = string("Failure to open file \"") + filename + "\"";
		Printer::getInst()->log(LOG_ERR, errStr);
		ErrorUtils_includeReason(model, REASON_NO_FILE);
	}
}

//==============================================================================
// Checks if opening file for reading was successful.
bool FileReader::isReady(void) {
	// Report result of opening file (trust an open file stays open).
	return m_fileOpened;
}

//==============================================================================
// Get name of the file being read from.
std::string FileReader::getFilename(void) {
	// Report name given on instantiation.
	return m_filename;
}

//==============================================================================
// Peeks next available byte in file (if opened successfully).
uint8_t FileReader::peekByte(void) {
	// Peeked byte to return.
	uint8_t retByte;

	// Call OS to peek next byte from file.
	if (!OsFile_peekByte(&m_file, &retByte)) {
		// File suddenly unavailable?! assert!
		Printer::getInst()->printAssert("file unavailable for peeking");
		Terminate::getInst()->exit(REASON_ASSERT);
	}

	// Return peeked byte.
	return retByte;
}

//==============================================================================
// Pops next available byte in file (if opened successfully).
uint8_t FileReader::popByte(void) {
	// Popped byte to return;
	uint8_t retByte;

	// Call OS to pop next byte from file.
	if (!OsFile_popByte(&m_file, &retByte)) {
		// File suddenly unavailable?! assert!
		Printer::getInst()->printAssert("file unavailable for popping");
		Terminate::getInst()->exit(REASON_ASSERT);
	}

	// Return popped byte.
	return retByte;
}

//==============================================================================
// Destructs the FileReader instance, closing the file in the process.
FileReader::~FileReader(void) {
	// Ensure file is closed before destruction.
	OsFile_closeFile(&m_file);
}
