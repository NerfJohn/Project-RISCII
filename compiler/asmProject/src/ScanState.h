/*
 * ScanState.h
 *
 *  Created on: Jan 10, 2024
 *      Author: John
 */

#ifndef SRC_SCANSTATE_H_
#define SRC_SCANSTATE_H_

#include <stdint.h>
#include "SyntaxToken_e.h"

// TODO- (sub)type of current scan state value.
typedef enum ScanStateType_e {
	SCAN_STATE_SEARCHING = 0,
	SCAN_STATE_SCANNING  = 1,
	SCAN_STATE_TOKEN     = 2,
	SCAN_STATE_ERROR     = 3,
} ScanStateType_e;

// TODO- "enum" value tracking state of a scan.
class ScanState {
public:
	// TODO- constructor; resets value of scan state.
	ScanState(void);

	// TODO- resets value of scan state to start.
	void reset(void);

	// TODO- moves to next state- reports type for robustness.
	ScanStateType_e nextState(uint8_t peekChar);

	// TODO- reports type of current state
	ScanStateType_e asType(void);

	// TODO- report scan state as token type (as possible).
	SyntaxToken_e asToken(void);

private:
	// TODO- actual "state" of the value.
	int m_rawState;
};


#endif /* SRC_SCANSTATE_H_ */
