/*
 * ItemToken.cpp: Tokenized excerpt of a file- result of lexing said file.
 */

#include "Ds/ItemToken.h"

using namespace std;

//==============================================================================
// TODO
BuildType_e ItemToken::getType(void) {
	// Build type directly derived from token.
	return (BuildType_e)(m_lexTkn);
}
