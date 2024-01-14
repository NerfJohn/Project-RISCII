/*
 * ScanState.cpp
 *
 *  Created on: Jan 10, 2024
 *      Author: John
 */

#include "ScanState.h"

using namespace std;

// TODO- macros explicitly for .nextState() (ie uses peekChar).
#define IS(a) (a == peekChar)
#define IN(a,b) ((a <= peekChar) && (peekChar <= b))
#define LBL (IN('a','z') || IN('A','Z') || IN('0','9') || IS('_'))
#define HEX (IN('a', 'f') || IN('A', 'F') || IN('0', '9'))

// TODO- base value to extend SyntaxToken_e.
static constexpr int STATE_BASE = TOKEN_VAL_MAX + 1;

// TODO- special sub-state values extended from tokens.
static constexpr int _SUB_ERROR   = STATE_BASE;
static constexpr int _SUB_START   = STATE_BASE + 1;
static constexpr int _SUB_VAL_MIN = _SUB_START;
static constexpr int _SUB_VAL_MAX = STATE_BASE + 85;

// TODO- constructor; resets value of scan state.
ScanState::ScanState(void) {
	// Reset internal state- begin fresh.
	this->reset();
}

// TODO- resets value of scan state to start.
void ScanState::reset(void) {
	// Starting sub state- empty buffer, no peek char.
	m_rawState = _SUB_START;
}

// TODO- moves to next state- reports type for robustness.
ScanStateType_e ScanState::nextState(uint8_t peekChar) {

	//////////////////////////////////////////////////////////////
	// Scan "Table": Switch statement to allow g++ to optimize. //
	//////////////////////////////////////////////////////////////

	// Run state's transition logic.
	int nextState = _SUB_ERROR; // No edge? To error state...
	switch (m_rawState) {
    case _SUB_START:
        if (IS('S')) {nextState = 34; break;}
        if (IS('A')) {nextState = 35; break;}
        if (IS('L')) {nextState = 36; break;}
        if (IS('J')) {nextState = 37; break;}
        if (IS('O')) {nextState = 38; break;}
        if (IS('X')) {nextState = 39; break;}
        if (IS('B')) {nextState = 40; break;}
        if (IS('N')) {nextState = 41; break;}
        if (IS('H')) {nextState = 42; break;}
        if (IS('_')) {nextState = 43; break;}
        if (IS('.')) {nextState = 44; break;}
        if (IS('$')) {nextState = 45; break;}
        if (IS('"')) {nextState = 46; break;}
        if (IS(':')) {nextState = 47; break;}
        if (IS('a')) {nextState = 48; break;}
        if (IS('s')) {nextState = 48; break;}
        if (IS('n')) {nextState = 49; break;}
        if (IS('z')) {nextState = 50; break;}
        if (IS('p')) {nextState = 51; break;}
        if (IS('c')) {nextState = 48; break;}
        if (IS('+')) {nextState = 52; break;}
        if (IS('-')) {nextState = 52; break;}
        if (IS('0')) {nextState = 53; break;}
        if (IN('1','9')) {nextState = 54; break;}
        if (IN('a','z')) {nextState = 55; break;}
        if (IN('A','Z')) {nextState = 55; break;}
        if (IS('_')) {nextState = 55; break;}
        if (IS('\t')) {nextState = _SUB_START; break;}
        if (IS(' ')) {nextState = _SUB_START; break;}
        if (IS('\n')) {nextState = _SUB_START; break;}
        if (IS('\r')) {nextState = _SUB_START; break;}
        if (IS(';')) {nextState = 56; break;}
        if (IS(255)) {nextState = TOKEN_EOF; break;}
        nextState = _SUB_ERROR; break;
    case 52:
        if (IN('0','9')) {nextState = 54; break;}
        nextState = _SUB_ERROR; break;
    case 53:
        if (IS('x')) {nextState = 57; break;}
        if (IN('0','9')) {nextState = 54; break;}
        nextState = TOKEN_IMMEDIATE; break;
    case 57:
        if (HEX) {nextState = 58; break;}
        nextState = _SUB_ERROR; break;
    case 45:
        if (IN('0','7')) {nextState = 59; break;}
        if (IS('r')) {nextState = 60; break;}
        if (IS('a')) {nextState = 61; break;}
        if (IS('b')) {nextState = 61; break;}
        if (IS('f')) {nextState = 62; break;}
        if (IS('s')) {nextState = 62; break;}
        nextState = _SUB_ERROR; break;
    case 63:
        if (IS('n')) {nextState = 46; break;}
        if (IS('r')) {nextState = 46; break;}
        if (IS('t')) {nextState = 46; break;}
        if (IS('0')) {nextState = 46; break;}
        if (IS('\\')) {nextState = 46; break;}
        if (IS('"')) {nextState = 46; break;}
        if (IS('x')) {nextState = 64; break;}
        nextState = _SUB_ERROR; break;
    case 64:
        if (HEX) {nextState = 65; break;}
        nextState = _SUB_ERROR; break;
    case 65:
        if (HEX) {nextState = 66; break;}
        if (IS('\\')) {nextState = 63; break;}
        if (IS('"')) {nextState = 67; break;}
        nextState = 46; break;
    case 66:
        if (IS('\\')) {nextState = 63; break;}
        if (IS('"')) {nextState = 67; break;}
        nextState = 46; break;
    case 49:
        if (IS('z')) {nextState = 50; break;}
        if (IS('p')) {nextState = 51; break;}
        if (IS('c')) {nextState = 48; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_FLAG; break;
    case 50:
        if (IS('p')) {nextState = 51; break;}
        if (IS('c')) {nextState = 48; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_FLAG; break;
    case 51:
        if (IS('c')) {nextState = 48; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_FLAG; break;
    case 48:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_FLAG; break;
    case 55:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LABEL; break;
    case 54:
        if (IN('0','9')) {nextState = 54; break;}
        nextState = TOKEN_IMMEDIATE; break;
    case 58:
        if (HEX) {nextState = 58; break;}
        nextState = TOKEN_IMMEDIATE; break;
    case 46:
        if (IS('\\')) {nextState = 63; break;}
        if (IS('"')) {nextState = 67; break;}
        nextState = 46; break;
    case 56:
        if (IS('\n')) {nextState = TOKEN_COMMENT; break;}
        nextState = 56; break;
    case 34:
        if (IS('H')) {nextState = 68; break;}
        if (IS('U')) {nextState = 69; break;}
        if (IS('T')) {nextState = 70; break;}
        if (IS('W')) {nextState = 71; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LABEL; break;
    case 68:
        if (IS('L')) {nextState = 72; break;}
        if (IS('R')) {nextState = 73; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LABEL; break;
    case 69:
        if (IS('B')) {nextState = 74; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LABEL; break;
    case 70:
        if (IS('R')) {nextState = 75; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LABEL; break;
    case 71:
        if (IS('P')) {nextState = 76; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LABEL; break;
    case 35:
        if (IS('N')) {nextState = 77; break;}
        if (IS('D')) {nextState = 78; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LABEL; break;
    case 77:
        if (IS('D')) {nextState = 79; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LABEL; break;
    case 78:
        if (IS('D')) {nextState = 80; break;}
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LABEL; break;
    case 36:
        if (IS('B')) {nextState = 81; break;}
        if (IS('D')) {nextState = 82; break;}
        nextState = TOKEN_LABEL; break;
    case 81:
        if (IS('I')) {nextState = 83; break;}
        nextState = TOKEN_LABEL; break;
    case 82:
        if (IS('R')) {nextState = 84; break;}
        nextState = TOKEN_LABEL; break;
    case 37:
        if (IS('P')) {nextState = 85; break;}
        if (IS('L')) {nextState = 86; break;}
        nextState = TOKEN_LABEL; break;
    case 85:
        if (IS('R')) {nextState = 87; break;}
        nextState = TOKEN_LABEL; break;
    case 86:
        if (IS('R')) {nextState = 88; break;}
        nextState = TOKEN_LABEL; break;
    case 38:
        if (IS('R')) {nextState = 89; break;}
        nextState = TOKEN_LABEL; break;
    case 89:
        if (IS('R')) {nextState = 90; break;}
        nextState = TOKEN_LABEL; break;
    case 39:
        if (IS('O')) {nextState = 91; break;}
        nextState = TOKEN_LABEL; break;
    case 91:
        if (IS('R')) {nextState = 92; break;}
        nextState = TOKEN_LABEL; break;
    case 40:
        if (IS('R')) {nextState = 93; break;}
        nextState = TOKEN_LABEL; break;
    case 93:
        if (IS('C')) {nextState = 94; break;}
        nextState = TOKEN_LABEL; break;
    case 41:
        if (IS('O')) {nextState = 95; break;}
        nextState = TOKEN_LABEL; break;
    case 95:
        if (IS('P')) {nextState = 96; break;}
        nextState = TOKEN_LABEL; break;
    case 42:
        if (IS('L')) {nextState = 97; break;}
        nextState = TOKEN_LABEL; break;
    case 97:
        if (IS('T')) {nextState = 98; break;}
        nextState = TOKEN_LABEL; break;
    case 43:
        if (IS('l')) {nextState = 99; break;}
        if (IS('s')) {nextState = 100; break;}
        if (IS('d')) {nextState = 101; break;}
        if (IS('t')) {nextState = 102; break;}
        nextState = TOKEN_LABEL; break;
    case 99:
        if (IS('d')) {nextState = 103; break;}
        nextState = TOKEN_LABEL; break;
    case 100:
        if (IS('t')) {nextState = 104; break;}
        nextState = TOKEN_LABEL; break;
    case 101:
        if (IS('s')) {nextState = 105; break;}
        nextState = TOKEN_LABEL; break;
    case 102:
        if (IS('o')) {nextState = 106; break;}
        nextState = TOKEN_LABEL; break;
    case 44:
        if (IS('w')) {nextState = 107; break;}
        if (IS('a')) {nextState = 108; break;}
        nextState = TOKEN_LABEL; break;
    case 107:
        if (IS('o')) {nextState = 109; break;}
        nextState = TOKEN_LABEL; break;
    case 109:
        if (IS('r')) {nextState = 110; break;}
        nextState = TOKEN_LABEL; break;
    case 110:
        if (IS('d')) {nextState = 111; break;}
        nextState = TOKEN_LABEL; break;
    case 108:
        if (IS('r')) {nextState = 112; break;}
        if (IS('d')) {nextState = 113; break;}
        nextState = TOKEN_LABEL; break;
    case 112:
        if (IS('r')) {nextState = 114; break;}
        nextState = TOKEN_LABEL; break;
    case 114:
        if (IS('y')) {nextState = 115; break;}
        nextState = TOKEN_LABEL; break;
    case 113:
        if (IS('d')) {nextState = 116; break;}
        nextState = TOKEN_LABEL; break;
    case 116:
        if (IS('r')) {nextState = 117; break;}
        nextState = TOKEN_LABEL; break;
    case 60:
        if (IN('0','3')) {nextState = 59; break;}
        nextState = _SUB_ERROR; break;
    case 61:
        if (IS('x')) {nextState = 59; break;}
        nextState = _SUB_ERROR; break;
    case 62:
        if (IS('p')) {nextState = 59; break;}
        nextState = _SUB_ERROR; break;
    case 72:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_SHL; break;
    case 73:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_SHR; break;
    case 74:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_SUB; break;
    case 75:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_STR; break;
    case 76:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_SWP; break;
    case 79:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_AND; break;
    case 80:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_ADD; break;
    case 83:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LBI; break;
    case 84:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LDR; break;
    case 87:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_JPR; break;
    case 88:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_JLR; break;
    case 90:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_ORR; break;
    case 92:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_XOR; break;
    case 94:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_BRC; break;
    case 96:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_NOP; break;
    case 98:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_HLT; break;
    case 103:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_LD; break;
    case 104:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_ST; break;
    case 105:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_DS; break;
    case 106:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_TO; break;
    case 111:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_WORD; break;
    case 115:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_ARRY; break;
    case 117:
        if (LBL) {nextState = 55; break;}
        nextState = TOKEN_ADDR; break;
    case 59:
        if (!LBL) {nextState = TOKEN_REGISTER; break;}
        nextState = _SUB_ERROR; break;
    case 67:
        nextState = TOKEN_STRING; break;
    case 47:
        nextState = TOKEN_COLON; break;
		default: // Non-transition state- already set to error...
			break;
	}

	// Update state.
	m_rawState = nextState;

	// Posterity: report new type.
	return this->asType();
}

// TODO- reports type of current state
ScanStateType_e ScanState::asType(void) {
	// Check if within token range.
	if ((TOKEN_VAL_MIN <= m_rawState) && (m_rawState <= TOKEN_VAL_MAX)) {
		return SCAN_STATE_TOKEN;
	}

	// Check if still searching for a token/notable characters.
	if (m_rawState == _SUB_START) {
		return SCAN_STATE_SEARCHING;
	}

	// Check if scanning a potential token (ie excludes internal error state).
	if ((_SUB_VAL_MIN <= m_rawState) && (m_rawState <= _SUB_VAL_MAX)) {
		return SCAN_STATE_SCANNING;
	}

	// Otherwise, an error.
	return SCAN_STATE_ERROR;
}

// TODO- report scan state as token type (as possible).
SyntaxToken_e ScanState::asToken(void) {
	// Report as invalid if not within proper range.
	if (this->asType() != SCAN_STATE_TOKEN) {
		return TOKEN_INVALID;
	}

	// Otherwise, return as casted token.
	return (SyntaxToken_e)(m_rawState);
}
