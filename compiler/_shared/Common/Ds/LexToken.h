/*
 * LexToken.h: Token derived from lexing a file.
 */

#ifndef COMMON_DS_LEXTOKEN_H_
#define COMMON_DS_LEXTOKEN_H_

#include "Common/Ds/IBuildItem.h"

/*
 * @brief Lexed "word" of file. Usable for building user defined nodes.
 *
 * Token with extra info for error/debug handling. Primary building block for
 * parsing (with Common/Device/Parser). Extends parent to include token's raw
 * value (e.g. strin literal's value).
 */
class LexToken: public IBuildItem {
public:
	// Raw contents lexed into token.
	std::string m_rawData;
	
	/* file/type info stored with IBuildItem vars */
};

#endif /* COMMON_DS_LEXTOKEN_H_ */
