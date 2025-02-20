/*
 * Lexer.h: Iteratively lexes pre-opened file.
 */

#ifndef COMMON_DEVICE_LEXER_H_
#define COMMON_DEVICE_LEXER_H_

#include <string>
#include "Common/Domain/LexInit_t.h"

/*
 * @brief Lexer core for lexing an opened file into user defined tokens.
 *
 * Assumes file opened with Common/Device/File. Performs core lexing algorithm
 * with callback functions to assist with user defined ruleset. Works similar to
 * C's standard "getopt" function.
 */
class Lexer {
public:
	// Parsed data shared with user.
	int         m_tknCode; // 'token enum' with result of lexing
	std::string m_value;   // raw value from the file
	uint32_t    m_line;    // line number (since lex init) of token
	
	/*
	 * @brief Constructor- inits lexer for given input data.
	 *
	 * Preps to lex file (assumed opened with Common/Device/File). Assumes
	 * It is lexing from start of the file (ie line number = 1).
	 *
	 * @param initData data used to prepare lexing of opened file
	 */
	Lexer(LexInit_t const& initData);
	
	/*
	 * @brief Lexes file for one token. Returns for token or lexing error.
	 *
	 * Lexes opened file, starting from given starting state. Does not save line
	 * number or characters until given starting state is left. Shared data is
	 * updated upon return.
	 */
	void lex(void);

private:
	// Internal data saved between calls.
	uint32_t m_curLine; // current line number (vs line of lexed token).
	
	// Callback/handlers defined by user.
	int  (*m_nextState)(int const, std::string const&, uint8_t const);
	bool (*m_isTkn)(int const);
	
	// Specific values from 'lex enum' required for lexing.
	int m_lexStart; // start state of all lex paths
	int m_lexError; // error state for bad lex path
};

#endif /* COMMON_DEVICE_LEXER_H_ */
