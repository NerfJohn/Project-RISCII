/*
 * File.cpp: Standardizes file operations (albeit one file at a time).
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Os/OsFile.h"

#include "Common/Device/File.h"

using namespace std;

//==============================================================================

// Local metrics for assertions/available info.
bool     s_isOpen = false;        // naturally closed at start
string   s_name   = "";           // (dummy value)
FileOp_e s_dir    = FILE_OP_READ; // (dummy value)

//==============================================================================
// Returns name of (opened) file. Call ONLY when file is open.
std::string& File_getName(void) {
	// Name requires an actively open file.
	if (s_isOpen == false) {
		Print_bug("getName() with closed file");
		Terminate_bug(); // Does NOT return
	}

	// Otherwise, return the file's name.
	return s_name;
}

//==============================================================================
// Opens given file for given operation.
RetErr_e File_open(std::string const& name, FileOp_e const dir) {
	// Cannot open a new file before closing last one.
	if (s_isOpen == true) {
		Print_bug("open() without closing file");
		Terminate_bug(); // Does NOT return
	}

	// Open requested file.
	RetErr_e res = OsFile_open(name, dir);

	// Save file data as applicable.
	if (res == RET_ERR_NONE) {
		s_isOpen = true;
		s_name   = name;
		s_dir    = dir;
	}

	// Return result of process.
	return res;
}

//==============================================================================
// Peeks byte from file (open and FILE_OP_READ operation).
uint8_t File_peek(void) {
	// Peek requires opened read file.
	if ((s_isOpen == false) || (s_dir != FILE_OP_READ)) {
		Print_bug("peek() without open read file");
		Terminate_bug(); // Does NOT return
	}

	// Peek file.
	uint8_t retByte = 0; // (dummy value)
	RetErr_e retErr = OsFile_peek(retByte);

	// Bad operation- something beyond program has happened.
	if(retErr) {
		Print_bug("peek() failed OS call");
		Terminate_bug(); // Does NOT return
	}

	// Return peeked byte.
	return retByte;
}

//==============================================================================
// Pops byte from file (open and FILE_OP_READ operation).
uint8_t File_pop(void) {
	// Pop requires opened read file.
	if ((s_isOpen == false) || (s_dir != FILE_OP_READ)) {
		Print_bug("pop() without open read file");
		Terminate_bug(); // Does NOT return
	}

	// Pop file.
	uint8_t retByte = 0; // (dummy value)
	RetErr_e retErr = OsFile_pop(retByte);

	// Bad operation- something beyond program has happened.
	if(retErr) {
		Print_bug("pop() failed OS call");
		Terminate_bug(); // Does NOT return
	}

	// Return peeked byte.
	return retByte;
}

//==============================================================================
// Writes data to file (open and FILE_OP_WRITE operation) (Option 1: byte).
void File_write(uint8_t const byte) {
	// Write requires opened write file.
	if ((s_isOpen == false) || (s_dir != FILE_OP_WRITE)) {
		Print_bug("write() without open write file");
		Terminate_bug(); // Does NOT return
	}

	// Write to file.
	RetErr_e retErr = OsFile_write(byte);

	// Bad operation- something beyond program has happened.
	if(retErr) {
		Print_bug("write() failed OS call");
		Terminate_bug(); // Does NOT return
	}
}

//==============================================================================
// Writes data to file (open and FILE_OP_WRITE operation) (Option 2: string).
void File_write(std::string const& str) {
	// Reuse "write byte" code.
	for (uint8_t byte : str) {File_write(byte);}
}

//==============================================================================
// Closes file (if open). Has no effect on closed file.
void File_close(void) {
	// Close current file.
	OsFile_close();

	// Update local metrics.
	s_isOpen = false;
}
