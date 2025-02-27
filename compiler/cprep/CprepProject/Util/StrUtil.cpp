/*
 * StrUtil.cpp: Utilities for querying/manipulating std strings.
 */

#include "Util/StrUtil.h"

using namespace std;

//==============================================================================

// Definitions for trimming whitespace.
#define WSPACE (" \n\r\t")

//==============================================================================
// Removes whitespace from "sides" of string. Ignore "middle" whitespace.
void StrUtil_trim(std::string& str) {
	// Trim each side of whitespace.
	str.erase(0, str.find_first_not_of(WSPACE)); // left
	str.erase(str.find_last_not_of(WSPACE) + 1); // right
}

//==============================================================================
// Removes all instances of character in string.
void StrUtil_rmAll(std::string& str, uint8_t chr) {
	// Prep to scan over characters.
	string tmp = str;
	       str = "";

	// Re-add all non-blacklisted chars.
	for (uint8_t byte: tmp) {if (byte != chr) {str += byte;}}
}
