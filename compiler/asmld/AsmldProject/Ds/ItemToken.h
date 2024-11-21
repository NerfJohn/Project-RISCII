/*
 * ItemToken.h: Tokenized excerpt of a file- result of lexing said file.
 */

#ifndef DS_ITEMTOKEN_H_
#define DS_ITEMTOKEN_H_

#include <cstdint>
#include <string>
#include "Domain/LexToken_e.h"

/*
 * Information about a lex token found within a file (ie "living" lex token).
 *
 * Effectively a lex token originating from a file- has a raw value and
 * file/line location. Primary value is the LexToken_e, though the other data
 * is kept for use in possible logged messages.
 */
class ItemToken {
public:
	// Data at location.
	LexToken_e  m_lexTkn;
	std::string m_rawStr;

	// Location of data.
	std::string m_file;
	uint32_t    m_line;
};

#endif /* DS_ITEMTOKEN_H_ */
