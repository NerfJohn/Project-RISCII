/*
 * FileReader.cpp
 *
 * " Device used to read input files byte for byte"
 */

#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "OsLayer/OsFile.h"

#include "DeviceLayer/FileReader.h"

using namespace std;

//==============================================================================
// Constructs file reader for given filename. File opened on construction.
FileReader::FileReader(std::string filename) {
	// Attempt to open the given filename for reading.
	m_fileOpened = OsFile_openReadFile(&m_file, filename);
}

//==============================================================================
// Checks if opening file for reading was successful.
bool FileReader::isReady(void) {
	// Report result of opening file (trust an open file stays open).
	return m_fileOpened;
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
