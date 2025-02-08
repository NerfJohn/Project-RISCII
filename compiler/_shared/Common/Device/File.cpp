/*
 * File.cpp: Standardizes file operations (albeit one file at a time).
 */

#include "Common/Device/Terminate.h"
#include "Common/Os/OsFile.h"

#include "Common/Device/File.h"

using namespace std;

//==============================================================================

// Local metrics for assertions/available info.
bool        File::s_isOpen = false;        // Naturally closed at start
std::string File::s_name   = "";
FileOp_e    File::s_dir    = FILE_OP_READ;

//==============================================================================
// Retrieves the singleton instance for operations.
File& File::inst(void) {
	// Static instance- only one is ever created.
	static File s_instance;

	// Return singleton.
	return s_instance;
}

//==============================================================================
// Returns name of (opened) file.
std::string File::getName(void) {
	// Name requires an actively open file.
	if (s_isOpen == false) {Terminate::inst().assert("getName() for no file");}

	// Otherwise, return the file's name.
	return s_name;
}

//==============================================================================
// Opens given file for given operation.
RetErr_e File::open(std::string const& name, FileOp_e const dir) {
	// Cannot open a new file before closing last one.
	if (s_isOpen == true) {
		Terminate::inst().assert("open() without closing file");
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
uint8_t File::peek(void) {
	// Byte to return.
	uint8_t retByte = 0xFF; // overwritten by OsFile regardless

	// Peek requires opened read file.
	if ((s_isOpen == false) || (s_dir != FILE_OP_READ)) {
		Terminate::inst().assert("peek() without open read file");
	}

	// Peek file.
	RetErr_e retErr = OsFile_peek(retByte);

	// Bad operation- something beyond program has happened.
	if(retErr) {Terminate::inst().assert("peek() failed OS call");}

	// Return peeked byte.
	return retByte;
}

//==============================================================================
// Pops byte from file (open and FILE_OP_READ operation).
uint8_t File::pop(void) {
	// Byte to return.
	uint8_t retByte = 0xFF; // overwritten by OsFile regardless

	// Pop requires opened read file.
	if ((s_isOpen == false) || (s_dir != FILE_OP_READ)) {
		Terminate::inst().assert("pop() without open read file");
	}

	// Pop file.
	RetErr_e retErr = OsFile_pop(retByte);

	// Bad operation- something beyond program has happened.
	if(retErr) {Terminate::inst().assert("pop() failed OS call");}

	// Return peeked byte.
	return retByte;
}

//==============================================================================
// Writes data to file (open and FILE_OP_WRITE operation) (Option 1: byte).
void File::write(uint8_t const byte) {
	// Write requires opened write file.
	if ((s_isOpen == false) || (s_dir != FILE_OP_WRITE)) {
		Terminate::inst().assert("write() without open write file");
	}

	// Write to file.
	RetErr_e retErr = OsFile_write(byte);

	// Bad operation- something beyond program has happened.
	if(retErr) {Terminate::inst().assert("write() failed OS call");}
}

//==============================================================================
// Writes data to file (open and FILE_OP_WRITE operation) (Option 2: string).
void File::write(std::string const& str) {
	// Reuse "write byte" code.
	for (uint8_t byte : str) {this->write(byte);}
}

//==============================================================================
// Closes file (if open). Has no effect on closed file.
void File::close(void) {
	// Close current file.
	OsFile_close();

	// Update local metrics.
	s_isOpen = false;
}
