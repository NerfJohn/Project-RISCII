/*
 * Lexer.h: Class containing language specific lexing rules/support.
 */

#ifndef DEVICE_LEXER_H_
#define DEVICE_LEXER_H_

#include <cstdint>
#include "Domain/LexToken_e.h"
#include "Domain/LexState_e.h"

/*
 * Returns the given state as a LexToken. Returns TOKEN_INVALID if not a token.
 *
 * Since LexState_e is a super set of LexToken_e, it sometimes can be casted as
 * a LexToken_e. This function implements the logic to safely check/do so.
 *
 * @param state lex state to turn into a lex token
 * @return      lex token if successful, TOKEN_INVALID otherwise
 */
LexToken_e Lexer_asToken(LexState_e const state);

/*
 * Returns next lex state for given current lex state and input.
 *
 * Encapsulates state transition logic for programming language lexing. Does not
 * implement state machine itself- only reports next state based on a starting
 * state and input (as if input was "peeked" from file).
 *
 * @param state lex state transition starts from
 * @param byte  byte input causing the state transition
 * @return      lex state transitions ends on
 */
LexState_e Lexer_nextState(LexState_e const state, uint8_t const byte);

#endif /* DEVICE_LEXER_H_ */
