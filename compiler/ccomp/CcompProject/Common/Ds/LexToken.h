/*
 * LexToken.h: Token derived from lexing a file.
 */

#ifndef COMMON_DS_LEXTOKEN_H_
#define COMMON_DS_LEXTOKEN_H_

#include "Common/Ds/IBuildItem.h"

/*
 * @brief Lexed "word" from file. Usable for building user defined nodes.
 *
 * "Leaves" of AST (built/parsed with Common/Device/Parser). Extends IBuildItem
 * to include raw value of token (ie chars making up token). Intended to be
 * created using Common/Device/Lexer.
 */
class LexToken: public IBuildItem {
public:
	// Items specific to token build item.
	std::string m_rawData; // raw characters making up token

	/*
	 * @brief Constructor- inits basic data.
	 *
	 * @param buffer raw string contents of token
	 * @param type   specific build piece (ie type of token)
	 * @param file   file of origin
	 * @param line   line (number) or origin
	 */
	LexToken(std::string buffer, int type, std::string file, uint32_t line) {
		// Init.
		m_rawData = buffer;
		m_type    = type;
		m_file    = file;
		m_line    = line;
	}
};

#endif /* COMMON_DS_LEXTOKEN_H_ */
