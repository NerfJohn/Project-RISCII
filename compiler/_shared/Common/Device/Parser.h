/*
 * Parser.h: Iteratively parses lex tokens.
 */

#ifndef COMMON_DEVICE_PARSER_H_
#define COMMON_DEVICE_PARSER_H_

#include <stack>
#include "Common/Domain/ParserInit_t.h"

/*
 * @brief Parser core for parsing an ordered list of lexed tokens.
 *
 * Performs core parsing algorithm with callback function to assist with user
 * defined ruleset. Works similar to C's standard "getopt" function.
 */
class Parser {
public:
	// Parsed data shared with the user.
	int                     m_actCode;  // 'parse enum' with resulting action
	std::stack<IBuildItem*> m_actStack; // stack of items to execute action on
	
	/*
	 * @brief Constructor- inits parser for given input data.
	 *
	 * Preps to parse lex tokens (ie Common/Ds/LexToken). Assumes tokens
	 * represent entire file- from start to finish.
	 *
	 * @param initData data used to prepare parsing of tokens
	 */
	Parser(ParserInit_t const& initData);

	/*
	 * @brief Parses tokens until node/"action" can be created from act stack.
	 *
	 * Pushes tokens to m_actStack while grammar checking. Returns when tokens
	 * can create a user defined node, updating the shared data prior to return.
	 * On error, pushes the error-causing token to top of m_actStack.
	 */
	void parse(void);
	
private:
	// Internal data saved between calls.
	std::stack<int> m_parseStack;
	
	// Input tokens to parse.
	std::queue<LexToken*> m_tkns;
	
	// Callback/handler functions defined by user.
	ParseOp_e (*m_parse)(std::vector<int> const**, int, int);
	bool      (*m_isTkn)(int);
	bool      (*m_isAct)(int);
	
	// Specific values from 'parse enum' required for parse.
	int m_parseStart;
	int m_parseMismatch;
	int m_parseUnexpected;
};

#endif /* COMMON_DEVICE_PARSER_H_ */
