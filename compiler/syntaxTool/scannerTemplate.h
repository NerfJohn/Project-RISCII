/*
 * ScanState.h
 * 
 * "Auto-generated class to handle scanner state transitions/deciphering.
 */

#include <stdbool>
#include <stdint>

// TODO
typedef enum ScanToken_e {
	@1
} ScanToken_e;

// TODO
typedef enum ScanStateType_e {
	SCAN_STATE_SCANNING,
	SCAN_STATE_TOKEN,
	SCAN_STATE_EOF,
	SCAN_STATE_ERROR
} ScanStateType_e;

// TODO
class ScanState {
public:
	// TODO
	ScanState(void);
	
	// TODO
	void reset(void);
	
	// TODO
	ScanStateType_e nextState(uint8_t peekChar);
	
	// TODO
	ScanStateType_e asType(void);
	
	// TODO
	ScanToken_e asToken(void);

private:
	// TODO
	@2
};