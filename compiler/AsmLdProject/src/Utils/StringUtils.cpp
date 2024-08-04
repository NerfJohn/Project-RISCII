/*
 * StringUtils.cpp
 *
 * "Helper functions for handling strings"
 */

#include <set>

#include "Utils/StringUtils.h"

using namespace std;

//==============================================================================

// Definitions for converting decimal and hexadecimal strings.
#define HEX_PREFIX     ("0x")
#define HEX_BASE       (16)
#define DEC_BASE       (10)

// Definitions for recognized escape chars.
#define ESC_NULL       ('0')
#define ESC_BSLASH     ('\\')
#define ESC_QUOTE      ('"')
#define ESC_NEWLINE    ('n')
#define ESC_RETURN     ('r')
#define ESC_TAB        ('t')

// Definitions for extracting parts of filenames.
#define FILE_DELIMITER ('.')

//==============================================================================
// Generates list of repeated chars in given string.
std::vector<uint8_t> StringUtils_getRepeatChars(std::string inText) {
	// Sets to track chars seen vs seen repeating.
	set<uint8_t> seen;
	set<uint8_t> repeated;

	// Look at each char in the given string.
	for (size_t i = 0; i < inText.size(); i++) {
		// Extract character.
		uint8_t chr = inText.at(i);

		// Mark as repeated if in set.
		if ((seen.find(chr) != seen.end())) {
			repeated.insert(chr); // saves once- even for 3+ of one char.
		}

		// Char seen- now it's in the set.
		seen.insert(chr);
	}

	// Return repeated chars.
	vector<uint8_t> retVec;
	copy(repeated.begin(), repeated.end(), back_inserter(retVec));
	return retVec;
}

//==============================================================================
// Removes duplicate chars from a given string. Retains first instances/order.
std::string StringUtils_getNoRepeats(std::string inText) {
	// String to return to caller.
	string retStr;

	// Set to track seen chars.
	set<uint8_t> seen;

	// Check each char in input string.
	for (size_t i = 0; i < inText.size(); i++) {
		// Extract character.
		uint8_t chr = inText.at(i);

		// Copy to return string if not repeated.
		if ((seen.find(chr) == seen.end())) {
			retStr += ((char)(chr));
		}

		// Char seen- now it's in the set.
		seen.insert(chr);
	}

	// Return "slimmed" string.
	return retStr;
}

//==============================================================================
// Creates list of chars used for unrecognized escapes in given string.
RetErr_e StringUtils_getBadEscapes(std::string strLit, std::string& chars) {
	// Return code indicating success of process.
	RetErr_e retCode = RET_GOOD; // innocent till guilty

	// Search string for escape chars.
	bool prevCharBslash = false; // no char checked yet
	for (size_t i = 0; i < strLit.size(); i++) {
		// Get char from string.
		char curChar = strLit[i];

		// Add bad escape chars to the list (as applicable).
		if (prevCharBslash &&
			(curChar != ESC_NULL) &&
			(curChar != ESC_BSLASH) &&
			(curChar != ESC_QUOTE) &&
			(curChar != ESC_NEWLINE) &&
			(curChar != ESC_RETURN) &&
			(curChar != ESC_TAB)) {
			// Add escape char.
			chars += curChar;
		}

		// Factor for backslashes (skipping over escaped ones).
		prevCharBslash = (!prevCharBslash && (curChar == ESC_BSLASH));
	}

	// Catch "open escapes" (can't directly imply with string).
	if (prevCharBslash) {retCode = RET_FAIL;}

	// Return indication of process result.
	return retCode;
}

//==============================================================================
// Converts string into bytes. Handles escape chars and ignores wrapping quotes.
RetErr_e StringUtils_asBytes(std::string const strLit,
		                     std::vector<uint8_t>& bytes) {
	// Return code indicating success of process.
	RetErr_e retCode = RET_GOOD; // innocent till guilty

	// Add each byte in order.
	bool prevCharBslash = false; // no char checked yet
	for (size_t i = 0; i < strLit.size(); i++) {
		// Get char from string.
		char curChar = strLit[i];

		// Add char (factoring for escaped chars as needed).
		if (!prevCharBslash) {
			// Typical char (unless start of escape or 'wrapping' quotes).
			if ((curChar != ESC_BSLASH) && (curChar != ESC_QUOTE)) {
				bytes.push_back((uint8_t)(curChar));
			}
		}
		else {
			// Escaped char- append "translated" escape.
			switch (curChar) {
				case ESC_NULL:    bytes.push_back('\0'); break;
				case ESC_BSLASH:  bytes.push_back('\\'); break;
				case ESC_QUOTE:   bytes.push_back('\"'); break;
				case ESC_NEWLINE: bytes.push_back('\n'); break;
				case ESC_RETURN:  bytes.push_back('\r'); break;
				case ESC_TAB:     bytes.push_back('\t'); break;
				default:
					// Unrecognized escape- failure.
					retCode = RET_FAIL;
			}
		}

		// Factor for backslashes (skipping over escaped ones).
		prevCharBslash = (!prevCharBslash && (curChar == ESC_BSLASH));
	}

	// Catch "open escapes" (can't directly imply with string).
	if (prevCharBslash) {retCode = RET_FAIL;}

	// Return indication of process result.
	return retCode;
}

//==============================================================================
// Converts string-ified uint into a typical uint. Handles decimal and hex.
bool StringUtils_asUint(std::string uintStr, uint32_t* uint) {
	// Value to return.
	bool retBool = true; // start with optimism

	// Attempt to convert the string into desired uint.
	try {
		if (uintStr.find(HEX_PREFIX, 0) != string::npos) {
			*uint = stoul(uintStr, 0, HEX_BASE);
		}
		else {
			*uint = stoul(uintStr, 0, DEC_BASE);
		}
	}
	catch (exception &e) {
		// Something went wrong- failure to convert.
		retBool = false;
	}

	// Return based on success of actual conversion.
	return retBool;
}

//==============================================================================
// Converts string-ified int into a typical int. Handles decimal and hex.
bool StringUtils_asInt(std::string intStr, int32_t* intPtr) {
	// Value to return.
	bool retBool = true; // start with optimism

	// Attempt to convert the string into desired int.
	try {
		if (intStr.find(HEX_PREFIX, 0) != string::npos) {
			*intPtr = stol(intStr, 0, HEX_BASE);
		}
		else {
			*intPtr = stol(intStr, 0, DEC_BASE);
		}
	}
	catch (exception &e) {
		// Something went wrong- failure to convert.
		retBool = false;
	}

	// Return based on success of actual conversion.
	return retBool;
}

//==============================================================================
// Removes "type" portion from filename (based on dot operator).
std::string StringUtils_removeFileType(std::string filename) {
	// Find the dot separating the file name and type.
	uint8_t dotIdx = filename.find(FILE_DELIMITER);

	// Return file's name only (no dot? index defaults to end of string anyway).
	return filename.substr(0, dotIdx);
}
