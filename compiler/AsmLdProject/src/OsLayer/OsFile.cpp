/*
 * OsFile.cpp
 *
 * "Wrapper for OS specific file interactions"
 */

#include "OsLayer/OsFile.h"

using namespace std;

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
