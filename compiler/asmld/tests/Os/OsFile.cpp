#include "tests/Os/OsExit.h"

#include "Os/OsFile.h"
#include "tests/Os/OsFile.h"

using namespace std;

// Dummy file.
static string s_name = "";
static string s_data = "";

// Response to use for OsFile functions.
static RetErr_e s_resp = RET_ERR_NONE;

//==============================================================================
// (Source code declared function).
RetErr_e OsFile_open(std::string const& name, FileOp_e const dir) {
	// Prep file.
	if (!OsExit_hasRet()) {
		s_name = name;
		if (dir == FILE_OP_WRITE) {s_data = "";}
	}

	// Return preset response.
	return s_resp;
}

//==============================================================================
// (Source code declared function).
RetErr_e OsFile_peek(uint8_t& byte) {
	// Read first character.
	if (!OsExit_hasRet()) {
		byte = 0xFF; // EOF
		if (!s_data.empty()) {byte = s_data[0];}
	}

	// Return preset response.
	return s_resp;
}

//==============================================================================
// (Source code declared function).
RetErr_e OsFile_pop(uint8_t& byte) {
	// Read/pop first character.
	if (!OsExit_hasRet()) {
		byte = 0xFF; // EOF
		if (!s_data.empty()) {
			byte = s_data[0];
			s_data.erase(s_data.begin());
		}
	}

	// Return preset response.
	return s_resp;
}

//==============================================================================
// (Source code declared function).
RetErr_e OsFile_write(uint8_t const byte) {
	//  Append character.
	if (!OsExit_hasRet()) {
		s_data.push_back(byte);
	}

	// Return preset response.
	return s_resp;
}

//==============================================================================
// (Source code declared function).
void OsFile_close(void) {
	/* No test-related actions */
}

//==============================================================================
// Setup dummy read file (overwrites previous file).
void OsFile_setFile(std::string name, std::string data) {
	// Simple overwrite.
	s_name = name;
	s_data = data;
}

//==============================================================================
// General setting to control how OsFile functions respond.
void OsFile_setResponse(RetErr_e resp) {
	s_resp = resp;
}

//==============================================================================
// Get the dummy file's current name.
std::string OsFile_getName(void) {return s_name;}

//==============================================================================
// Get the dummy file's curent data.
std::string OsFile_getData(void) {return s_data;}
