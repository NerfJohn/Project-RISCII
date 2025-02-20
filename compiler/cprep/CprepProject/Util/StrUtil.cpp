/*
 * StrUtil.cpp: Utilities for querying/manipulating std strings.
 */

#include "Util/StrUtil.h"

using namespace std;

//==============================================================================

// Definitions for trimming whitespace.
#define WSPACE (" \n\r\t")

//==============================================================================
// Removes whitespace from "sides" of given string. Ignore "middle" whitespace.
void StrUtil_trim(std::string& str) {
	// Trim each side of whitespace.
	str.erase(0, str.find_first_not_of(WSPACE)); // left
	str.erase(str.find_last_not_of(WSPACE) + 1); // right
}
