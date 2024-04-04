/*
 * AsmToken.h
 *
 *  Created on: Feb 2, 2024
 *      Author: John
 */

#ifndef SRC_ASMTOKEN_H_
#define SRC_ASMTOKEN_H_


#include <string>

#include "../Parsing/SyntaxToken_e.h"

// TODO- Compiler defined grouping of characters.
class AsmToken {
public:
	// TODO- Instances must always save "who/where" values to be useful.
	AsmToken(SyntaxToken_e type, std::string value, std::string origin);

	// TODO- print for ease of debugging.
	std::string asStr(void);

	// Typical "getters".
	SyntaxToken_e getType(void);
	std::string getValue(void);
	std::string getOrigin(void);

	// Std. destructor.
	~AsmToken(void);

private:
	// TODO- General "read-only" data of token.
	SyntaxToken_e m_type;  // compiler's definition of the grouping
	std::string m_value;   // raw value read from the file
	std::string m_origin;  // file/line origin of the raw value
};

#endif /* SRC_ASMTOKEN_H_ */
