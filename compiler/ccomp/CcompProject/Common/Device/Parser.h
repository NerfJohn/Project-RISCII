/*
 * Parser.h: Iteratively parses lex tokens.
 */

#ifndef COMMON_DEVICE_PARSER_H_
#define COMMON_DEVICE_PARSER_H_

#include <stack>
#include <queue>
#include "Common/Ds/IBuildItem.h"
#include "Common/Ds/LexToken.h"
#include "Common/Util/Ptr.h"

/*
 * @brief Parser core to parse stream of tokens into AST.
 *
 * Iterates over tokens- grouping tokens (and nodes) into larger nodes. Assumes
 * given list of tokens represents a full file. Relies on user to own rule-set
 * and node creation (though provides action stack).
 */
class Parser {
public:
	// Parsed data shared with the user.
	int                         m_actCode;  // parse action to take
	std::stack<Ptr<IBuildItem>> m_actStack; // items to execute action on

	/*
	 * @brief Constructor- preps to parse token list into actions/nodes.
	 *
	 * @param tkns list of input tokens to parse
	 */
	Parser(std::queue<Ptr<LexToken>>& tkns);

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
	std::stack<int>            m_parseStack; // expected states/tokens
	std::queue<Ptr<LexToken>>  m_tkns;       // input tokens
};

#endif /* COMMON_DEVICE_PARSER_H_ */
