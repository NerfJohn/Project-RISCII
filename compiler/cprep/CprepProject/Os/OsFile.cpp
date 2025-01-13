/*
 * OsFile.cpp: Abstraction of a (single) file in OS file system.
 */

#include <fstream>

#include "Os/OsFile.h"

using namespace std;

//==============================================================================

// Current "file" controlled by the functions.
static fstream  s_file;
static FileOp_e s_dir;

//==============================================================================
// Opens file with the specified path/name and direction.
RetErr_e OsFile_open(std::string const& name, FileOp_e const dir) {
	// GUILTY till innocent.
	RetErr_e res = RET_ERR_ERROR;

	// Can only open a file if NOT already in use.
	if (!s_file.is_open()) {
		// Save direction of opened file.
		s_dir = dir;

		// Attempt to open file.
		ios_base::openmode dirFlag = (dir == FILE_OP_READ) ? ios::in : ios::out;
		s_file.open(name, ios::binary | dirFlag);

		// Alter result if file is open.
		if (s_file.is_open()) {res = RET_ERR_NONE;}
	}

	// Return result of process.
	return res;
}

//==============================================================================
// Peeks next char from file (if current file is open and reading).
RetErr_e OsFile_peek(uint8_t& byte) {
	// GUILTY till innocent.
	RetErr_e res = RET_ERR_ERROR;

	// Attempt to peek byte.
	if ((s_dir == FILE_OP_READ) && (s_file.is_open())) {
		// Peek byte (underlying OS returns 0xFF if at end of file).
		byte = s_file.peek();

		// Verdict- "Innocent".
		res = RET_ERR_NONE;
	}

	// Return result of process.
	return res;
}

//==============================================================================
// Pops next char from file (if current file is open and reading).
RetErr_e OsFile_pop(uint8_t& byte) {
	// GUILTY till innocent.
	RetErr_e res = RET_ERR_ERROR;

	// Attempt to pop byte.
	if ((s_dir == FILE_OP_READ) && (s_file.is_open())) {
		// Pop byte (underlying OS returns 0xFF if at end of file).
		byte = s_file.get();

		// Verdict- "Innocent".
		res = RET_ERR_NONE;
	}

	// Return result of process.
	return res;
}

//==============================================================================
// Writes byte to (end of) file (if current file is open and writing).
RetErr_e OsFile_write(uint8_t const byte) {
	// GUILTY till innocent.
	RetErr_e res = RET_ERR_ERROR;

	// Attempt to write byte.
	if ((s_dir == FILE_OP_WRITE) && (s_file.is_open())) {
		// Write byte.
		s_file.write((char const*)(&byte), sizeof(uint8_t));

		// Verdict- "Innocent".
		res = RET_ERR_NONE;
	}

	// Return result of process.
	return res;
}

//==============================================================================
// Closes any file currently open. Returns without error regardless of process.
void OsFile_close(void) {
	// Close file (as applicable).
	if (s_file.is_open()) {s_file.close();}
}
