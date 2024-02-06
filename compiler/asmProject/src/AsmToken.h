/*
 * AsmToken.h
 *
 *  Created on: Feb 2, 2024
 *      Author: John
 */

#ifndef SRC_ASMTOKEN_H_
#define SRC_ASMTOKEN_H_

#include <string>
#include "SyntaxToken_e.h"

// TODO- desc.
class AsmToken {
public:
	// TODO- desc.
	AsmToken(SyntaxToken_e type, std::string value = "");

	// TODO- desc.
	SyntaxToken_e getType(void);

	// TODO- desc.
	std::string getValue(void);

private:
	// TODO- desc.
	SyntaxToken_e m_type;
	std::string m_value;
};

#endif /* SRC_ASMTOKEN_H_ */
