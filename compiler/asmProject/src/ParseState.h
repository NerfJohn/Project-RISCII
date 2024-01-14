/*
 * ParseState.h
 *
 *  Created on: Jan 13, 2024
 *      Author: John
 */

#ifndef SRC_PARSESTATE_H_
#define SRC_PARSESTATE_H_

#include <stack>
#include "ParseAction_e.h"

// TODO- enum desribing general state of parsing.
typedef enum ParseStateType_e {
	PARSE_STATE_PARSING  = 0,
	PARSE_STATE_ACTION   = 1,
	PARSE_STATE_MISMATCH = 2,
	PARSE_STATE_NO_MATCH = 3,
	PARSE_STATE_EMPTY    = 4,
} ParseStateType_e;

// TODO- class tracking state of parsing.
class ParseState {
public:
	// TODO- constructor; resets value of scan state.
	ParseState(void);

	// TODO- resets value of scan state to start.
	void reset(void);

	// TODO- moves to next state- reports type for robustness.
	ParseStateType_e nextState(SyntaxToken_e popTkn);

	// TODO- pops parse state as action type (if possible, abort if not action).
	ParseAction_e popAction(void);

	// TODO- reports type of current state
	ParseStateType_e asType(void);

	// TODO- report parse state as token type (as possible).
	SyntaxToken_e asToken(void);

private:
	// TODO- raw stack/state of parsing.
	std::stack<int> m_rawState;

	// TODO- indicator of bad parsing.
	bool m_inError;
};


#endif /* SRC_PARSESTATE_H_ */
