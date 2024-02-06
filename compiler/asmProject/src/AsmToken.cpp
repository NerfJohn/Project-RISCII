/*
 * AsmToken.cpp
 *
 *  Created on: Feb 2, 2024
 *      Author: John
 */

#include "AsmToken.h"

using namespace std;

// TODO- desc.
AsmToken::AsmToken(SyntaxToken_e type, std::string value) {
	// Save passed data.
	m_type = type;
	m_value = value;
}

// TODO- desc.
SyntaxToken_e AsmToken::getType(void) {
	return m_type;
}

// TODO- desc.
std::string AsmToken::getValue(void) {
	return m_value;
}
