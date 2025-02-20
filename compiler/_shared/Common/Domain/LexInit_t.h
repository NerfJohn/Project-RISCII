/*
 * LexInit_t.h: Initial data required to create Lexer.
 */

#ifndef COMMON_DOMAIN_LEXINIT_T_H_
#define COMMON_DOMAIN_LEXINIT_T_H_

#include <cstdint>
#include <string>

/*
 * @brief Stores data required to initialize a Lexer instance.
 * 
 * Intended to be used with Common/Device/Lexer.h. Should be filled in using
 * program's passed in argument and program-tailored handler functions. See
 * Common/Util/LexerDefs.h for help making those functions.
 */
typedef struct {
	// Callback/handler functions defined by user.
	int  (*m_nextState)(int const, std::string const&, uint8_t const);
	bool (*m_isTkn)(int const);
	
	// Specific values from 'lex enum' required for lexing.
	int m_lexStart; // start state of all lex paths
	int m_lexError; // error state for bad lex path
} LexInit_t;

#endif /* COMMON_DOMAIN_LEXINIT_T_H_ */
