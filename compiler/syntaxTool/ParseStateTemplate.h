/*
 * ScanState.h
 * 
 * "Auto-generated header- to include once in application's scanner cpp file."
 * 
 * Author: genScanState.py (auto-generated)
 */

/*
 * Usage:
 *
 * This tool generates "ParseState" class that encapsulates a defined language's
 * parser state machine. A class instance can handle parse transition logic and
 * recognition while the application code handles what to do with the results.
 *
 * It is assumed that a "SyntaxToken_e.h" has been defined with enum values (of
 * prefix "TOKEN_") to define the application's tokens (including the values
 * TOKEN_MAX_VALUE and TOKEN_INVALID).
 *
 * Similarly, it is assumed that a "ParseAction_e.h" has been defined with enum
 * values (of prefix "ACTION_") to define the application's tokens (including the
 * values ACTION_MAX_VALUE and ACTION_INVALID).
 *
 * It is also assumed the generated header file is included only once in the
 * application program- in the .cpp file handling with the application's "Parse
 * Phase" code. Including more than once may cause compilation errors.
 */

#ifndef PARSESTATE_H_
#define PARSESTATE_H_

////////////////////////////////////////////////////////////////////////////////
// User/Application Code Relevant Information- to be used by the application  //
////////////////////////////////////////////////////////////////////////////////

#include <stack>
#include "ParseAction_e.h" // Application definitions for ACTION_ enum names
#include "SyntaxToken_e.h" // Application definitions for TOKEN_ enum names

//============================================================================//

/*
 * High-level sub-states of ParseState. To be referenced by application code.
 * 
 * Types represent 1+ ParseState sub-states, referring to the broader steps of
 * parsing (that the application may care about). The application may use
 * these values to guide data/control actions of the parsing process.
 */
typedef enum {
	PARSE_TYPE_PARSING  = 0,
	PARSE_TYPE_ACTION   = 1,
	PARSE_TYPE_MISMATCH = 2,
	PARSE_TYPE_NO_MATCH = 3,
	PARSE_TYPE_EMPTY    = 4,
} ParseStateType_e;

//============================================================================//

/*
 * Class encapsulating parse transition logic. Acts similar to a "super enum".
 * 
 * Instances track state machine progress of a parsing. Can be checked for
 * general status of parse and report token types as their parsed. Transition
 * logic determined by rules given to auto-generator script.
 */
class ParseState {
public:
	/*
	 * Basic constructor. Instance is fully reset upon instantiation.
	 */
	ParseState(void);

	/*
	 * Resets the instances state- as if no tokens have been popped.
	 */
	void reset(void);

	/*
	 * Updates state based on next token (assumes token was popped, NOT peeked).
	 * 
	 * @param popTkn popped token to use to update current state
	 * @return new state type of the instance
	 */
	ParseStateType_e nextState(SyntaxToken_e popTkn);

	/*
	 * Pops parse state as parse action. Cancels pop if NOT a parse action.
	 * 
	 * Pop only executes when the state is a parse action. Otherwise, the state
	 * is not changed by calling this method.
	 * 
	 * @return parse action if state type is valid, otherwise ACTION_INVALID
	 */
	ParseAction_e popAction(void);

	/*
	 * Returns state type of the current state, describing the overall status.
	 * 
	 * @return state type of the instance
	 */
	ParseStateType_e asType(void);

	/*
	 * Returns state as scan token. Invalid if state type isn't a token.
	 * 
	 * @return scanned token if state type is valid, otherwise TOKEN_INVALID
	 */
	SyntaxToken_e asToken(void);

private:
	// Saved "state" value- uses stack for "sub-sentence" parsing. 
	std::stack<int> m_rawState;
	
	// Tracks validity of parsing/grammar, regardless of state(s).
	bool m_inError;
};

////////////////////////////////////////////////////////////////////////////////
// Internal/Scan Transition Logic Information- NOT for application use!       //
////////////////////////////////////////////////////////////////////////////////

/*
 * Active sub-states of ParseState. NOT intended for application code use.
 * 
 * Sub-states of ParseState. Defined on top of ParseAction_e types to allow
 * SyntaxToken_e, ParseAction_e, and sub-state mixing. These states are (mostly)
 * non-terminal and should NOT be referenced by application code.
 */
typedef enum {
	@enumStates // auto-generated (PARSE_SUB_<name> = ACTION_MAX_VALUE + #)
} ParseSubState_e;

//============================================================================//

/*
 * Sub-state "strings" added to ParseState's internal state per transition.
 * 
 * All valid sub-state vectors contained as one block of values. Vectors are
 * extracted by hard-coding starting index of vector, then reading until
 * PARSE_SUB_NULL is hit (ie "null terminated string").
 */
static int PARSE_PATTERNS[] = {
	@patternList // auto-generated (<item>,<item>,...)
};

//============================================================================//

// Basic constructor. Instance is fully reset upon instantiation.
ParseState::ParseState(void) {
	// Reset internal state- begin fresh.
	this->reset();
}

//============================================================================//

// Resets the instances state- as if no tokens have been popped.
void ParseState::reset(void) {
	// Clear-out states.
	while(m_rawState.size()) {m_rawState.pop();}

	// Starting sub state- "start" token parsed into entire program.
	m_rawState.push(PARSE_SUB_start);
	m_inError = false;
}

//============================================================================//

// Updates state based on next token (assumes token was popped, NOT peeked).
ParseStateType_e ParseState::nextState(SyntaxToken_e popTkn) {
	// Cannot transition while in error.
	if (m_inError) {return this->asType();}

	// Skip over actions (not involved in transitions).
	while (this->asType() == PARSE_TYPE_ACTION) {
		m_rawState.pop();
	}

	// Empty state? No transitions/matches -> error.
	if (m_rawState.size() == 0) {
		m_inError = true;
		return PARSE_TYPE_NO_MATCH;
	}

	// State is token? Try to match.
	if (this->asToken() != TOKEN_INVALID) {
		if (m_rawState.top() == popTkn) {
			// Match- pop and move on.
			m_rawState.pop();
			return this->asType();
		}
		else {
			// Mismatch- to error state.
			m_inError = true;
			return PARSE_TYPE_MISMATCH;
		}
	}
	
	// Otherwise, sub-state: get index of component states.
	int nextIdx = -1; // Assume invalid index/error for now...
	switch(m_rawState.top()) {
		@caseData // auto-generated (if (<clause>) {nextIdx = <idx>;})
		default: // Non-transition state- already set to error...
			break;
	}

	// Error if no index set.
	if (nextIdx < 0) {
		m_inError = true;
		return PARSE_TYPE_NO_MATCH;
	}

	// Otherwise, a match- update state(s).
	m_rawState.pop();
	while(PARSE_PATTERNS[nextIdx] != PARSE_SUB_NULL) {
		m_rawState.push(PARSE_PATTERNS[nextIdx]);
		nextIdx++;
	}

	// For robustness: report new type.
	return this->asType();
}

//============================================================================//

// Pops parse state as parse action. Cancels pop if NOT a parse action.
ParseAction_e ParseState::popAction(void) {
	// Cannot pop when in error.
	if (m_inError) {return ACTION_INVALID;}
	
	// - No error- pop action in ideal conditions - //
	
	// Check top of stack for state within "action range".
	if (m_rawState.size() && 
		(TOKEN_MAX_VALUE < m_rawState.top()) &&
		(m_rawState.top() <= ACTION_MAX_VALUE)) {
		// Confirmed action- pop and present.
		ParseAction_e res = (ParseAction_e)(m_rawState.top());
		m_rawState.pop();
		return res;
	}
	
	// Otherwise, not an action- invalidate (with NO pop).
	return ACTION_INVALID;
}

//============================================================================//

// Returns state type of the current state, describing the overall status.
ParseStateType_e ParseState::asType(void) {
	// Error overrides all other types.
	if (m_inError) {
		// Mis-matched if token left in state machine.
		if (m_rawState.size() &&
			(m_rawState.top() <= TOKEN_MAX_VALUE)) {
			return PARSE_TYPE_MISMATCH;
		}
		
		// Otherwise, no match was made (leading to error).
		return PARSE_TYPE_NO_MATCH;
	}
	
	// - No error- determine type in ideal conditions - //

	// First check if empty (saves following conditions a safety check).
	if (m_rawState.size() == 0) {
		return PARSE_TYPE_EMPTY;
	}

	// Check if an action (that needs executing).
	if ((TOKEN_MAX_VALUE < m_rawState.top()) &&
		(m_rawState.top() <= ACTION_MAX_VALUE)) {
		return PARSE_TYPE_ACTION;
	}

	// Otherwise, we're just parsing along.
	return PARSE_TYPE_PARSING;
}

//============================================================================//

// Returns state as scan token. Invalid if state type isn't a token.
SyntaxToken_e ParseState::asToken(void) {
	// Cannot check for token when in error.
	if (m_inError) {return TOKEN_INVALID;}
	
	// - No error- report as token in ideal conditions - //
	
	// Check current state is within token boundaries.
	if (m_rawState.size() &&
		(m_rawState.top() <= TOKEN_MAX_VALUE)) {
		return (SyntaxToken_e)(m_rawState.top());
	}
		
	// Otherwise, state is NOT a token.
	return TOKEN_INVALID;
}

#endif /* SCANSTATE_H_ */
