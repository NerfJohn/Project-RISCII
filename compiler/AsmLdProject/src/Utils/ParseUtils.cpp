/*
 * ParseUtils.cpp
 *
 * "Helper functions for parsing scan tokens for assembly language"
 */

#include  <vector>

#include "Utils/ParseUtils.h"

using namespace std;

//==============================================================================

// Definitions for easing reading parse conditions.
#define IS(x)     if ((x) == token)
#define ELSE      if (true)

// Definitions for easing reading replacement selection.
#define USE(x)    {nextStates = &(x); break;}
#define STATES(x) vector<ParseState_e> const x
#define ITEM(x)   ((ParseState_e)(x))

// Constants containing replacements for parsed states.
STATES(PATTERN_EPSILON)     = {};
STATES(PATTERN_REGISTER)    = {ITEM(TOKEN_REGISTER)};
STATES(PATTERN_LABEL_DECL)  = {ITEM(PARSE_START),
                               ITEM(ACTION_BUILD_LABEL_DECL),
                               ITEM(TOKEN_COLON)
                              };
STATES(PATTERN_SHR_INSTR)   = {ITEM(PARSE_START),
		                       ITEM(ACTION_BUILD_INSTRUCTION),
		                       ITEM(PARSE_3RD_OP),
					           ITEM(TOKEN_REGISTER),
                               ITEM(PARSE_OPT_FLAG)
                              };

//==============================================================================
// Returns the given state as a LexToken. Returns TOKEN_INVALID if not a token.
LexToken_e ParseUtils_asLexToken(ParseState_e state) {
	// Can cast it if it's within a lex token's range.
	LexToken_e stateAsToken = (LexToken_e)(state);
	if ((TOKEN_LEX_MIN <= stateAsToken) && (stateAsToken <= TOKEN_LEX_MAX)) {
		return stateAsToken;
	}

	// Otherwise, it becomes an unknown/invalid token type.
	return TOKEN_INVALID;
}

//==============================================================================
// Returns the given state as a ParseAction. Returns ACTION_INVALID on failure.
ParseAction_e ParseUtils_asParseAction(ParseState_e state) {
	// Can cast it if it's within a parse action's range.
	ParseAction_e stateAsAction = (ParseAction_e)(state);
	if ((ACTION_PARSE_MIN <= stateAsAction) &&
	    (stateAsAction <= ACTION_PARSE_MAX)) {
		return stateAsAction;
	}

	// Otherwise, it becomes an unknown/invalid action type.
	return ACTION_INVALID;
}

//==============================================================================
// Parses top state of stack into its components for given token input.
bool ParseUtils_parseTop(std::stack<ParseState_e>* stack, LexToken_e token) {
	// Ensure stack can be read.
	if ((stack == nullptr) || (stack->empty())) {
		// Stack top doesn't even exist- fail to parse.
		return false;
	}

	// Ensure top of stack needs to be parsed.
	ParseState_e topState = stack->top();
	if ((ParseUtils_asLexToken(topState) != TOKEN_INVALID) ||
		(ParseUtils_asParseAction(topState) != ACTION_INVALID)) {
		// "Parsed" top of stack- successfully parsed.
		return true;
	}

	// Determine sequence of states to replace top of stack.
	vector<ParseState_e> const * nextStates = nullptr;
	switch (stack->top()) {
		case PARSE_START:                                // sequence start
			IS(TOKEN_SHR)       USE(PATTERN_SHR_INSTR)
			IS(TOKEN_EOF)       USE(PATTERN_EPSILON)
			IS(TOKEN_LABEL)     USE(PATTERN_LABEL_DECL)
			break;
		case PARSE_OPT_FLAG:                             // optional flag
			IS(TOKEN_FLAGS)     USE(PATTERN_REGISTER)
			IS(TOKEN_REGISTER)  USE(PATTERN_EPSILON)
			break;
		case PARSE_3RD_OP:                               // 3rd op: reg vs imm
			IS(TOKEN_REGISTER)  USE(PATTERN_EPSILON)
			IS(TOKEN_IMMEDIATE) USE(PATTERN_EPSILON)
			break;
		default:
			// Failed to replace the state- failed to parse.
			nextStates = nullptr;
	}

	// Attempt to replace top state.
	if (nextStates != nullptr) {
		// Push replacement pattern first.
		stack->pop();
		for (size_t i = 0; i < nextStates->size(); i++) {
			stack->push(nextStates->at(i));
		}

		// Then push instigating token (guaranteed to be checked for next).
		stack->push((ParseState_e)(token));
	}

	// Return based on success in finding replace states.
	return (nextStates != nullptr);
}
