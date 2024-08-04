/*
 * ParseUtils.h
 *
 * "Helper functions for parsing scan tokens for assembly language"
 */

#ifndef SRC_UTILS_PARSEUTILS_H_
#define SRC_UTILS_PARSEUTILS_H_

#include <stack>
#include "DomainLayer/ParseState_e.h"

/*
 * Returns the given state as a LexToken. Returns TOKEN_INVALID if not a token.
 *
 * Since ParseState_e is a super set of LexToken_e, it sometimes can be casted
 * as a LexToken_e. This function implements the logic to safely check/do so.
 *
 * @param state parse state to turn into a lex token
 * @return lex token if successful, TOKEN_INVALID otherwise
 */
LexToken_e ParseUtils_asLexToken(ParseState_e state);

/*
 * Returns the given state as a ParseAction. Returns ACTION_INVALID on failure.
 *
 * Since ParseState_e is a super set of ParseAction_e, it sometimes can be
 * casted as a ParseAction. This function implements the logic to safely
 * check/do so.
 *
 * @param state parse state to turn into a parse action
 * @return parse action if successful, ACTION_INVALID otherwise
 */
ParseAction_e ParseUtils_asParseAction(ParseState_e state);

/*
 * Parses top state of stack into its components for given token input.
 *
 * Encapsulates parsing "breakdown" logic for assembly language parsing. Does
 * not implement state machine itself- just provides the service of breaking
 * down sub-states (assumes input token is 'peeked' from token stream).
 *
 * @param stack parse stack with a sub-state at its peak
 * @param token next token to parse (assuming it is 'peeked' from stream)
 * @return true for a successful breakdown/parsing, false otherwise
 */
bool ParseUtils_parseTop(std::stack<ParseState_e>* stack, LexToken_e token);

#endif /* SRC_UTILS_PARSEUTILS_H_ */
