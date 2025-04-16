/*
 * OsFile.cpp: Abstraction layer of a (single) file in OS file system.
 */

#include <fstream>

#include "Common/Os/OsFile.h"

using namespace std;

//==============================================================================

// Current "file" controlled by the functions.
static fstream  s_file;
static FileOp_e s_dir;

//==============================================================================
// Opens given file for given operation.
RetErr_e OsFile_open(std::string const& name, FileOp_e const dir) {
	// GUILTY till innocent.
	RetErr_e res = RET_ERR_ERROR;

	// Can only open a file if NOT already in use.
	if (s_file.is_open() == false) {
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
// Peeks byte from (open and reading) file.
RetErr_e OsFile_peek(uint8_t& byte) {
	// GUILTY till innocent.
	RetErr_e res = RET_ERR_ERROR;

	// Attempt to peek byte.
	if (s_file.is_open() && (s_dir == FILE_OP_READ)) {
		// Peek byte (underlying OS returns 0xFF if at end of file).
		byte = s_file.peek();

		// Verdict- "Innocent".
		res = RET_ERR_NONE;
	}

	// Return result of process.
	return res;
}

//==============================================================================
// Pops byte from (open and reading) file.
RetErr_e OsFile_pop(uint8_t& byte) {
	// GUILTY till innocent.
	RetErr_e res = RET_ERR_ERROR;

	// Attempt to pop byte.
	if (s_file.is_open() && (s_dir == FILE_OP_READ)) {
		// Pop byte (underlying OS returns 0xFF if at end of file).
		byte = s_file.get();

		// Verdict- "Innocent".
		res = RET_ERR_NONE;
	}

	// Return result of process.
	return res;
}

//==============================================================================
// Appends byte to (open and writing) file.
RetErr_e OsFile_write(uint8_t const byte) {
	// GUILTY till innocent.
	RetErr_e res = RET_ERR_ERROR;

	// Attempt to write byte.
	if (s_file.is_open() && (s_dir == FILE_OP_WRITE)) {
		// Write byte.
		s_file.write((char const*)(&byte), sizeof(uint8_t));

		// Verdict- "Innocent".
		res = RET_ERR_NONE;
	}

	// Return result of process.
	return res;
}

//==============================================================================
// Closes file (if open)- has no effect otherwise.
void OsFile_close(void) {
	// Close file (as applicable).
	if (s_file.is_open()) {s_file.close();}
}
