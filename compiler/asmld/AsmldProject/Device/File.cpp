/*
 * File.cpp: Standard portal for accessing files (albeit one at a time).
 */

#include "Device/Terminate.h"
#include "Os/OsFile.h"

#include "Device/File.h"

using namespace std;

//==============================================================================

// Local metrics for assertions/available info.
bool        File::m_isOpen = false; // Naturally closed at start
std::string File::m_name;
FileOp_e    File::m_dir;

//==============================================================================
// Retrieves the singleton instance for operations.
File& File::inst(void) {
	// Static instance- only one is ever created.
	static File m_instance;

	// Return singleton.
	return m_instance;
}

//==============================================================================
// Retrieves name of currently opened file. Should not be called otherwise.
std::string File::getName(void) {
	// Name requires an actively open file.
	if (m_isOpen == false) {
		Terminate_assert("Fetched filename without opening file");
	}

	// Otherwise, return the file's name.
	return m_name;
}

//==============================================================================
// Opens file for given operation. Returns based on success of operation.
RetErr_e File::open(std::string const& name, FileOp_e const dir) {
	// Cannot open a new file before closing last one.
	if (m_isOpen) {
		string bugStr = string("Opened '")   +
				        name                 +
						"' before closing '" +
						m_name               +
						"'";
		Terminate_assert(bugStr);
	}

	// Open requested file.
	RetErr_e res = OsFile_open(name, dir);

	// Save file data as applicable.
	if (res == RET_ERR_NONE) {
		m_isOpen = true;
		m_name   = name;
		m_dir    = dir;
	}

	// Return result of process.
	return res;
}

//==============================================================================
// Peeks byte from file (if open and reading).
uint8_t File::peek(void) {
	// Peek requires an actively open file.
	if (m_isOpen == false) {
		Terminate_assert("Peeked without opening file");
	}

	// Cannot peek a file opened for writing.
	if (m_dir == FILE_OP_WRITE) {
		Terminate_assert(string("Peeked write file '" + m_name + "'"));
	}

	// Peek file.
	uint8_t retByte = 0;
	RetErr_e retErr = OsFile_peek(retByte);

	// Bad operation- something beyond program has happened.
	if(retErr) {
		Terminate_assert(string("Unable to peek file '") + m_name + "'");
	}

	// Return peeked byte.
	return retByte;
}

//==============================================================================
// Pops byte from file (if open and reading).
uint8_t File::pop(void) {
	// Pop requires an actively open file.
	if (m_isOpen == false) {
		Terminate_assert("Popped without opening file");
	}

	// Cannot pop a file opened for writing.
	if (m_dir == FILE_OP_WRITE) {
		Terminate_assert(string("Popped write file '" + m_name + "'"));
	}

	// Pop file.
	uint8_t retByte = 0;
	RetErr_e retErr = OsFile_pop(retByte);

	// Bad operation- something beyond program has happened.
	if(retErr) {
		Terminate_assert(string("Unable to pop file '") + m_name + "'");
	}

	// Return popped byte.
	return retByte;
}

//==============================================================================
// Writes byte to file (if open and writing).
void File::write(uint8_t byte) {
	// Write requires an actively open file.
	if (m_isOpen == false) {
		Terminate_assert("Wrote without opening file");
	}

	// Cannot write a file opened for reading.
	if (m_dir == FILE_OP_READ) {
		Terminate_assert(string("Wrote read file '" + m_name + "'"));
	}

	// Peek file.
	RetErr_e retErr = OsFile_write(byte);

	// Bad operation- something beyond program has happened.
	if(retErr) {
		Terminate_assert(string("Unable to write file '") + m_name + "'");
	}
}

//==============================================================================
// Closes any open file.
void File::close(void) {
	// Close current file.
	OsFile_close();

	// Update local metrics.
	m_isOpen = false;
}
