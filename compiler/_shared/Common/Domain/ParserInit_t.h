/*
 * ParserInit_t.h: Initial data required to create parser.
 */

#ifndef COMMON_DOMAIN_PARSERINIT_T_H_
#define COMMON_DOMAIN_PARSERINIT_T_H_

#include <queue>
#include <vector>
#include "Common/Domain/ParseOp_e.h"
#include "Common/Ds/LexToken.h"

/*
 * @brief data required to create Common/Device/Parser.
 *
 * Includes "input", callbacks for program-specific operations, and key values
 * (again, program specific). Sets up parser instance to parse given tokens.
 */
typedef struct {
	// Input tokens to parse.
	std::queue<LexToken*> const& m_tkns;
	
	// Callback/handler functions defined by user.
	ParseOp_e (*m_parse)(std::vector<int> const**, int, int);
	bool      (*m_isTkn)(int);
	bool      (*m_isAct)(int);
	
	// Specific values from 'parse enum' required for parse.
	int m_parseStart;      // Start/root state for file parsing
	int m_parseMismatch;   // "Mistmatched token" error state
	int m_parseUnexpected; // "Unexpected token" error state
} ParserInit_t;

#endif /* COMMON_DOMAIN_PARSERINIT_T_H_ */
