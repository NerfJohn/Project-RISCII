/*
 * AsmToken.cpp
 *
 *  Created on: Feb 2, 2024
 *      Author: John
 */

#include "../Parsing/AsmToken.h"

#include <string>


using namespace std;

// TODO- Instances must always save "who/where" values to be useful.
AsmToken::AsmToken(SyntaxToken_e type, std::string value, std::string origin) {
	// Save the passed values- now "read-only".
	m_type = type;
	m_value = value;
	m_origin = origin;
}

// TODO- print for ease of debugging.
std::string AsmToken::asStr(void) {
	// Get prefix.
	string retStr = "{AsmToken: ";

	// Get contents of instance.
	retStr += "m_type(#)= " + to_string(m_type) + ", ";
	retStr += "m_value= \"" + m_value + "\", ";
	retStr += "m_origin = \"" + m_origin + "\"";

	// Get suffix.
	retStr += "}";

	// Return string-ified AsmToken.
	return retStr;
}

// Typical "getters".
SyntaxToken_e AsmToken::getType(void) {return m_type;}
std::string AsmToken::getValue(void) {return m_value;}
std::string AsmToken::getOrigin(void) {return m_origin;}

// Std. destructor.
AsmToken::~AsmToken(void) {/*No non-std pointers*/}
