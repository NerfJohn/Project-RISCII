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
