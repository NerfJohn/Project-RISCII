/*
 * ScanToken.cpp
 */

#include "ScanToken.h"

using namespace std;

/*
 * Note: String table below is vulnerable to changes to ScanTable.h's
 * ScanTableStates enumerator. Advised to either carefully monitor this table's
 * use w.r.t. the enum or implicitly correlate them in code.
 */

// Static table to help with converting object into a string.
const char* ScanToken::s_STR_TABLE[] = {
		"BAD_TKN",
		"&",
		"|",
		"^",
		"~",
		"+",
		"-",
		"{",
		"}",
		"(",
		")",
		";",
		",",
		">>",
		">=",
		"<<",
		"<=",
		"==",
		"!=",
		"C-LIT",
		"COMMENT",
		">",
		"<",
		"=",
		"!",
		"ID",
		"I-LIT",
		"H-LIT",
		"if",
		"int",
		"char",
		"while",
		"return",
		"unsigned"
};

// "Initialize" constructor for ScanToken object.
ScanToken::ScanToken(const ScanTableStates type,
			  	  	 const int lineNum,
					 const string value) {
	// Save off less contested variables.
	this->m_lineNum = lineNum;

	// Adjust type if given type is non-sensical.
	m_type = type; // Default "otherwise"
	if (type == SCAN_START) {
		m_type = SCAN_ERROR;	// Not a token, but conveys message
	}

	// Save value for identifiers/literals (ie requires extra data).
	m_value.empty(); // Default "otherwise"
	if ((type == SCAN_ID) ||
		(type == SCAN_ILITERAL) ||
		(type == SCAN_HLITERAL) ||
		(type == SCAN_CLITERAL)) {
		m_value = value;
	}
}

// Creates string version of ScanToken object.
string ScanToken::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add type using conversion table.
	tknStr += s_STR_TABLE[m_type - SCAN_ERROR]; // Normalize to index

	// Add line number.
	tknStr += " at " + to_string(m_lineNum);

	// Add value/data if applicable.
	if (m_value.size()) {tknStr += " = " + m_value;}

	// Finish punctuation and return.
	return tknStr + "}";
}
