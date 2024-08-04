/*
 * ScanToken_t.h
 *
 * "General data related to building block of assembly language"
 */

#ifndef SRC_DOMAINLAYER_SCANTOKEN_T_H_
#define SRC_DOMAINLAYER_SCANTOKEN_T_H_

#include <string>
#include "LexToken_e.h"

/*
 * Basic information of a token scanned from an input file.
 *
 * Includes data both type/value, as well as information on where it came from
 * (for warning/error reporting).
 */
typedef struct {
	// Raw Lex Data.
	LexToken_e  m_lexTkn;
	std::string m_rawStr;

	// Origin of token info.
	std::string m_orignFile;
	uint32_t    m_originLine;
} ScanToken_t;

#endif /* SRC_DOMAINLAYER_SCANTOKEN_T_H_ */
