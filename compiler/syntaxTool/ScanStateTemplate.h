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
 * This tool generates "ScanState" class that encapsulates a defined language's
 * scanner state machine. A class instance can handle scan transition logic and
 * recognition while the application code handles what to do with the results.
 * 
 * It is assumed that a "SyntaxToken_e.h" has been defined with enum values (of
 * prefix "TOKEN_") to define the application's tokens (including the values
 * TOKEN_MAX_VALUE and TOKEN_INVALID).
 * 
 * It is also assumed the generated header file is included only once in the
 * application program- in the .cpp file handling with the application's "Scan
 * Phase" code. Including more than once may cause compilation errors.
 */

#ifndef SCANSTATE_H_
#define SCANSTATE_H_

////////////////////////////////////////////////////////////////////////////////
// User/Application Code Relevant Information- to be used by the application  //
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "SyntaxToken_e.h" // Application definitions for TOKEN_ enum names

//============================================================================//

/*
 * High-level sub-states of ScanState. To be referenced by application code.
 * 
 * Types represent 1+ ScanState sub-states, referring to the broader steps of
 * scanning (that the application may care about). The application may use
 * these values to guide data/control actions of scanning process.
 */
typedef enum {
	SCAN_TYPE_SEARCHING = 0, // ScanState is searching for a token
	SCAN_TYPE_SCANNING  = 1, // ScanState is parsing a token
	SCAN_TYPE_TOKEN     = 2, // ScanState has completely parsed one token
	SCAN_TYPE_ERROR     = 3, // ScanState cannot create a token from the input
} ScanStateType_e;

//============================================================================//

/*
 * Class encapsulating scan transition logic. Acts similar to a "super enum".
 * 
 * Instances track state machine progress of a scan. Can be checked for general
 * status of scan and report token types as their scanned. Transition logic
 * determined by rules given to auto-generator script.
 */
class ScanState {
public:
	/*
	 * Basic constructor. Instance is fully reset upon instantiation.
	 */
	ScanState(void);

	/*
	 * Resets the instances state- as if no chars have been peeked.
	 */
	void reset(void);

	/*
	 * Updates state based on next char (assumes char was peeked, NOT popped).
	 * 
	 * @param peekChar peeked char to use to update current state
	 * @return new state type of the instance
	 */
	ScanStateType_e nextState(uint8_t peekChar);

	/*
	 * Returns state type of the current state, describing the overall status.
	 * 
	 * @return state type of the instance
	 */
	ScanStateType_e asType(void);

	/*
	 * Returns state as scan token. Invalid if state type isn't a token.
	 * 
	 * @return scanned token if state type is valid otherwise TOKEN_INVALID
	 */
	SyntaxToken_e asToken(void);

private:
	// Saved state value- actual "state" of the scan.
	int m_rawState;
};

////////////////////////////////////////////////////////////////////////////////
// Internal/Scan Transition Logic Information- NOT for application use!       //
////////////////////////////////////////////////////////////////////////////////

// Definitions/macros for common ScanState.nextState() conditions.
#define IS(x)    (x == peekChar)
#define IN(x,y)  ((x <= peekChar) && (peekChar <= y))
#define LBL_CHAR (IN('a', 'z') || IN('A', 'Z') || IN('0', '9') || IS('_'))
#define HEX_CHAR (IN('a', 'f') || IN('A', 'F') || IN('0', '9'))

//============================================================================//

/*
 * Active sub-states of ScanState. NOT intended for application code use.
 * 
 * Sub-states of ScanState. Defined on top of SytaxToken_e types to allow mixing
 * of the two in transition logic. These states are (mostly) non-terminal and
 * should NOT be referenced by application code.
 */
typedef enum {
	@enumStates // auto-generated (SCAN_SUB_<name> = TOKEN_MAX_VALUE + #)
} ScanSubState_e;

//============================================================================//

// Basic constructor. Instance is fully reset upon instantiation.
ScanState::ScanState(void) {
	// Reset upon instantiation.
	this->reset();
}

//============================================================================//

// Resets the instances state- as if no chars have been peeked.
void ScanState::reset(void) {
	// Back to starting state.
	m_rawState = SCAN_SUB_start;
}

//============================================================================//

// Updates state based on next char (assumes char was peeked, NOT popped).
ScanStateType_e ScanState::nextState(uint8_t peekChar) {
	// Next state of ScanState instance- assume error until proven wrong.
	int nextState = SCAN_SUB_ERROR;
	
	// Run transition logic (here we go!).
	switch (nextState) {
		@caseData // auto-generated (if (<clause>) {nextState = <dest>;})
		default: // Non-transition state- already set to error...
			break;
	}
	
	// Update state.
	m_rawState = nextState;
	
	// For robustness: report new type.
	return this->asType();
}

//============================================================================//

// Returns state type of the current state, describing the overall status.
ScanStateType_e ScanState::asType(void) {
	// Check if within token range.
	if (m_rawState <= TOKEN_MAX_VALUE) {
		return SCAN_TYPE_TOKEN;
	}

	// Check if still searching for a token/notable characters.
	if (m_rawState == SCAN_SUB_start) {
		return SCAN_TYPE_SEARCHING;
	}
	
	// Check if error state has been hit.
	if (m_rawState == SCAN_SUB_ERROR) {
		return SCAN_TYPE_ERROR;
	}
	
	// Otherwise, must be scanning a token.
	return SCAN_TYPE_SCANNING;
}

//============================================================================//

// Returns state as scan token. Invalid if state type isn't a token.
SyntaxToken_e ScanState::asToken(void) {
	// Report as invalid if not within proper range.
	if (m_rawState > TOKEN_MAX_VALUE) {
		return TOKEN_INVALID;
	}

	// Otherwise, return as casted token.
	return (SyntaxToken_e)(m_rawState);
}

#endif /* SCANSTATE_H_ */
