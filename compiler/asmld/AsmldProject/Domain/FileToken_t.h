/*
 * FileToken_t.h: Tokenized excerpt of a file- result of lexing said file.
 */

#ifndef DOMAIN_FILETOKEN_T_H_
#define DOMAIN_FILETOKEN_T_H_

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
typedef struct {
	// Data at location.
	LexToken_e  m_lexTkn;
	std::string m_rawStr;

	// Location of data.
	std::string m_file;
	uint32_t    m_line;
} FileToken_t;

#endif /* DOMAIN_FILETOKEN_T_H_ */
