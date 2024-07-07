/*
 * LexUtils.h
 *
 * "Helper functions for lexing an input assembly language file"
 */

#ifndef SRC_UTILS_LEXUTILS_H_
#define SRC_UTILS_LEXUTILS_H_

#include <cstdint>
#include "DomainLayer/LexState_e.h"
#include "DomainLayer/LexToken_e.h"

/*
 * Returns the given state as a LexToken. Returns TOKEN_INVALID if not a token.
 *
 * Since LexState_e is a super set of LexToken_e, it sometimes be casted as a
 * LexToken_e. This function implements the logic to safely check/do so.
 *
 * @param state lex state to turn into a lex token
 * @return lex token if successful, TOKEN_INVALID otherwise
 */
LexToken_e LexUtils_asLexToken(LexState_e state);

/*
 * Returns next lex state for given current lex state and input.
 *
 * Encapsulates state transition logic for assembly language lexing. Does not
 * implement state machine itself- only report next state based on a starting
 * state and input (as if input was "peeked" from file).
 *
 * @param state lex state transition starts from
 * @param byte byte input causing the state transition
 * @return lex state transitions ends on
 */
LexState_e LexUtils_nextState(LexState_e state, uint8_t byte);

#endif /* SRC_UTILS_LEXUTILS_H_ */
