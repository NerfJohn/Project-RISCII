/*
 * OsFile.cpp
 *
 * "Wrapper for OS specific file interactions"
 */

#include "OsLayer/OsFile.h"

using namespace std;

//==============================================================================

// Common definitions for status return codes.
#define RET_GOOD (0)
#define RET_ERR  (-1)

//==============================================================================
// Opens specified file for reading (assigned to ReadFile_t).
bool OsFile_openReadFile(ReadFile_t* file, std::string filename) {
	// Attempt to open the file.
	file->open(filename, ios::in | ios::binary);

	// Ensure it is open (failed if not).
	if (!file->is_open()) {return false;}

	// File successfully opened for binary reading.
	return true;
}

//==============================================================================
// Reads next available byte in file, but does not consume it/move on.
bool OsFile_peekByte(ReadFile_t* file, uint8_t* byte) {
	// Ensure file is still open (failed if not).
	if (!file->is_open()) {return false;}

	// Peek next byte.
	*byte = file->peek();

	// Successful peek.
	return true;
}

//==============================================================================
// Reads next available byte in file, consuming it/moving on in the process.
bool OsFile_popByte(ReadFile_t* file, uint8_t* byte) {
	// Ensure file is still open (failed if not).
	if (!file->is_open()) {return false;}

	// Pop next byte.
	*byte = file->get();

	// Successful pop.
	return true;
}

//==============================================================================
// Closes read file (if file passed in was open).
void OsFile_closeFile(ReadFile_t* file) {
	// Close file (as applicable).
	if (file->is_open()) {file->close();}
}

//==============================================================================
// Opens specified file for writing (assigned to WriteFile_t).
int OsFile_openWriteFile(WriteFile_t& file, std::string const filename) {
	// Return code to indicate result of operation.
	int retCode = RET_GOOD; // innocent till guilty

	// Attempt to open the file.
	file.open(filename, ios::out | ios::binary);

	// Ensure it is open (failed if not).
	if (!file.is_open()) {retCode = RET_ERR;}

	// Return result of operation.
	return retCode;
}

//==============================================================================
// Writes byte to file, appending it to the end of the file.
int OsFile_writeByte(WriteFile_t& file, uint8_t const byte) {
	// Return code to indicate result of operation.
	int retCode = RET_GOOD; // innocent till guilty

	// Attempt to write opened file.
	if (!file.is_open()) {retCode = RET_ERR;}
	else {file.write((char const*)(&byte), sizeof(uint8_t));}

	// Return result of operation.
	return retCode;
}

//==============================================================================
// Closes read file (if file passed in was open).
void OsFile_closeFile(WriteFile_t& file) {
	// Close file (as applicable).
	if (file.is_open()) {file.close();}
}
