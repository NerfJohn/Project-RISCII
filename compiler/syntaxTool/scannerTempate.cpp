/*
 * ScanState.cpp
 * 
 * "Auto-generated class to handle scanner state transitions/deciphering.
 */

#include "ScanState.h"

// TODO
ScanState::ScanState(void) {
	// New scanner state- start at reset value.
	this->reset();
}

// TODO
ScanState::reset(void) {
	// Reset internal state.
	this->m_rawState = @1
}

// TODO
ScanStateType_e ScanState::nextState(uint8_t peekChar) {
	
}

// TODO
ScanStateType_e ScanState::asType(void) {
	
}

// TODO
ScanToken_e ScanState::asToken(void) {
	
}