/*
 * ItemToken.h: Tokenized excerpt of a file- result of lexing said file.
 */

#ifndef DS_ITEMTOKEN_H_
#define DS_ITEMTOKEN_H_

#include "Ds/ABuildItem.h"

// TODO
/*
 * Information about a lex token found within a file (ie "living" lex token).
 *
 * Effectively a lex token originating from a file- has a raw value and
 * file/line location. Primary value is the LexToken_e, though the other data
 * is kept for use in possible logged messages.
 */
class ItemToken: public ABuildItem {
public:
	// Data at location.
	LexToken_e  m_lexTkn;
	std::string m_rawStr;

	// TODO
	BuildType_e getType(void);
};

#endif /* DS_ITEMTOKEN_H_ */
