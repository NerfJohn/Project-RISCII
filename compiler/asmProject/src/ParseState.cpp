/*
 * ParseState.cpp
 *
 *  Created on: Jan 13, 2024
 *      Author: John
 */

#include <iostream>
#include "ParseState.h"

using namespace std;

// TODO- base value to extend ParseAction_e (and SyntaxToken_e).
static constexpr int STATE_BASE = ACTION_VAL_MAX + 1;

// TODO- special sub-state values extended from tokens.
static constexpr int _SUB_NULL    = STATE_BASE + 0;
static constexpr int _SUB_START   = STATE_BASE + 1;

// TODO- "compressed" array of different patterns (separated by "nulls").
static int PARSE_PATTERNS[] = {
	51,52,TOKEN_REGISTER,TOKEN_REGISTER,50,51,53,TOKEN_REGISTER,TOKEN_REGISTER,
	50,51,53,TOKEN_REGISTER,54,50,51,ACTION_INSTR_RI8,TOKEN_IMMEDIATE,54,50,51,
	ACTION_INSTR_I8,55,50,51,ACTION_INSTR_RRI6,TOKEN_IMMEDIATE,TOKEN_REGISTER,
	TOKEN_REGISTER,50,51,ACTION_INSTR_RI5,TOKEN_IMMEDIATE,TOKEN_REGISTER,50,51,
	ACTION_INSTR_RRI5,TOKEN_IMMEDIATE,TOKEN_REGISTER,TOKEN_REGISTER,50,51,
	ACTION_INSTR_CMD,50,51,ACTION_MACRO_LD,TOKEN_LABEL,TOKEN_REGISTER,50,51,
	ACTION_MACRO_ST,TOKEN_LABEL,TOKEN_REGISTER,50,51,ACTION_MACRO_DS,
	TOKEN_REGISTER,50,51,ACTION_MACRO_TO,56,50,51,ACTION_WORD,TOKEN_IMMEDIATE,
	50,51,ACTION_ARRY,57,50,51,ACTION_ADDR,58,50,51,ACTION_LABEL_DECL,
	TOKEN_COLON,50,50,ACTION_INSTR_RRR,50,ACTION_INSTR_RRI5,50,
	ACTION_INSTR_RRI4,50,TOKEN_REGISTER,50,TOKEN_IMMEDIATE,50,TOKEN_LABEL,50,
};

// TODO- constructor; resets value of parse state.
ParseState::ParseState(void) {
	// Reset internal state- begin fresh.
	this->reset();
}

// TODO- resets value of parse state to start.
void ParseState::reset(void) {
	// Clear-out states.
	while(m_rawState.size()) {m_rawState.pop();}

	// Starting sub state- parse program file/start.
	m_rawState.push(_SUB_START);

	// Reset- clear error(s).
	m_inError = false;
}

ParseStateType_e ParseState::nextState(SyntaxToken_e popTkn) {
	// Cannot transition out of an error.
	if (m_inError) {return this->asType();}

	// Skip over actions (not involved in transitions).
	while (this->asType() == PARSE_STATE_ACTION) {
		m_rawState.pop();
	}

	// Empty state? No transitions/matches -> error.
	if (m_rawState.size() == 0) {
		m_inError = true;
		return PARSE_STATE_NO_MATCH;
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
			return PARSE_STATE_MISMATCH;
		}
	}

	// -- Line reached- must use parse table to determine states -- //

	///////////////////////////////////////////////////////////////
	// Parse "Table": Switch statement to allow g++ to optimize. //
	///////////////////////////////////////////////////////////////

	// Run state's transition logic.
	int nextIdx = -1;
	switch(m_rawState.top()) {
    case _SUB_START:
        if (popTkn == TOKEN_AND) {nextIdx = 0; break;}
        if (popTkn == TOKEN_ORR) {nextIdx = 0; break;}
        if (popTkn == TOKEN_XOR) {nextIdx = 0; break;}
        if (popTkn == TOKEN_ADD) {nextIdx = 0; break;}
        if (popTkn == TOKEN_SUB) {nextIdx = 0; break;}
        if (popTkn == TOKEN_SHL) {nextIdx = 5; break;}
        if (popTkn == TOKEN_SHR) {nextIdx = 10; break;}
        if (popTkn == TOKEN_LBI) {nextIdx = 15; break;}
        if (popTkn == TOKEN_BRC) {nextIdx = 20; break;}
        if (popTkn == TOKEN_LDR) {nextIdx = 24; break;}
        if (popTkn == TOKEN_STR) {nextIdx = 24; break;}
        if (popTkn == TOKEN_SWP) {nextIdx = 24; break;}
        if (popTkn == TOKEN_JPR) {nextIdx = 30; break;}
        if (popTkn == TOKEN_JLR) {nextIdx = 35; break;}
        if (popTkn == TOKEN_NOP) {nextIdx = 41; break;}
        if (popTkn == TOKEN_HLT) {nextIdx = 41; break;}
        if (popTkn == TOKEN_LD) {nextIdx = 44; break;}
        if (popTkn == TOKEN_ST) {nextIdx = 49; break;}
        if (popTkn == TOKEN_DS) {nextIdx = 54; break;}
        if (popTkn == TOKEN_TO) {nextIdx = 58; break;}
        if (popTkn == TOKEN_WORD) {nextIdx = 62; break;}
        if (popTkn == TOKEN_ARRY) {nextIdx = 66; break;}
        if (popTkn == TOKEN_ADDR) {nextIdx = 70; break;}
        if (popTkn == TOKEN_LABEL) {nextIdx = 74; break;}
        if (popTkn == TOKEN_EOF) {nextIdx = 78; break;}
        nextIdx = -1; break;
    case 52:
        if (popTkn == TOKEN_REGISTER) {nextIdx = 79; break;}
        if (popTkn == TOKEN_IMMEDIATE) {nextIdx = 81; break;}
        nextIdx = -1; break;
    case 53:
        if (popTkn == TOKEN_REGISTER) {nextIdx = 79; break;}
        if (popTkn == TOKEN_IMMEDIATE) {nextIdx = 83; break;}
        nextIdx = -1; break;
    case 54:
        if (popTkn == TOKEN_FLAG) {nextIdx = 85; break;}
        if (popTkn == TOKEN_REGISTER) {nextIdx = 78; break;}
        nextIdx = -1; break;
    case 55:
        if (popTkn == TOKEN_FLAG) {nextIdx = 87; break;}
        if (popTkn == TOKEN_IMMEDIATE) {nextIdx = 78; break;}
        nextIdx = -1; break;
    case 56:
        if (popTkn == TOKEN_REGISTER) {nextIdx = 89; break;}
        if (popTkn == TOKEN_LABEL) {nextIdx = 78; break;}
        nextIdx = -1; break;
    case 57:
        if (popTkn == TOKEN_STRING) {nextIdx = 78; break;}
        if (popTkn == TOKEN_IMMEDIATE) {nextIdx = 78; break;}
        nextIdx = -1; break;
    case 58:
        if (popTkn == TOKEN_IMMEDIATE) {nextIdx = 78; break;}
        if (popTkn == TOKEN_QUESTION) {nextIdx = 78; break;}
        nextIdx = -1; break;
	default: // Non-transition state- already set to error...
		break;
	}

	// Error if no index set.
	if (nextIdx < 0) {
		m_inError = true;
		return PARSE_STATE_NO_MATCH;
	}

	// Otherwise, a match- adjust state(s).
	m_rawState.pop();
	while(PARSE_PATTERNS[nextIdx] != _SUB_NULL) {
		m_rawState.push(PARSE_PATTERNS[nextIdx]);
		nextIdx++;
	}

	// Success- return proper type.
	return this->asType();
}

ParseAction_e ParseState::popAction() {
	// Cannot modify state while in error.
	if (m_inError) {return ACTION_INVALID;}

	// Check top of stack for an action.
	if (m_rawState.size() &&
		(ACTION_VAL_MIN <= m_rawState.top()) &&
		(m_rawState.top() <= ACTION_VAL_MAX)) {
		ParseAction_e ret = (ParseAction_e)(m_rawState.top());
		m_rawState.pop();
		return ret;
	}

	// Otherwise, couldn't convert (and did NOT pop).
	return ACTION_INVALID;
}

// TODO- reports type of current state
ParseStateType_e ParseState::asType(void) {
	// Error state overrides all others.
	if (m_inError) {
		// Token on top of stack implies mis-match.
		if (this->asToken() != TOKEN_INVALID) {
			return PARSE_STATE_MISMATCH;
		}

		// Otherwise- a lack of a match.
		return PARSE_STATE_NO_MATCH;
	}

	// First check if empty (saves following conditions a safety check).
	if (m_rawState.size() == 0) {
		return PARSE_STATE_EMPTY;
	}

	// Check if an action (that needs executing).
	if ((ACTION_VAL_MIN <= m_rawState.top()) &&
		(m_rawState.top() <= ACTION_VAL_MAX)) {
		return PARSE_STATE_ACTION;
	}

	// Otherwise, wer'e still just parsing along.
	return PARSE_STATE_PARSING;
}

// TODO- report parse state as token type (as possible).
SyntaxToken_e ParseState::asToken(void) {
	// Check top of stack for a token.
	if (m_rawState.size() &&
		(TOKEN_VAL_MIN <= m_rawState.top()) &&
		(m_rawState.top() <= TOKEN_VAL_MAX)) {
		return (SyntaxToken_e)(m_rawState.top());
	}

	// Otherwise, couldn't convert.
	return TOKEN_INVALID;
}
