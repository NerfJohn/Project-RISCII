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
 * these values to guide data/control actions of the scanning process.
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
 * status of scan and report token types as they're scanned. Transition logic
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
	 * @return scanned token if state type is valid, otherwise TOKEN_INVALID
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
#define EOF_CHAR (255 == peekChar)

//============================================================================//

/*
 * Active sub-states of ScanState. NOT intended for application code use.
 * 
 * Sub-states of ScanState. Defined on top of SyntaxToken_e types to allow
 * mixing of the two in transition logic. These states are (mostly) non-terminal
 * and should NOT be referenced by application code.
 */
typedef enum {
    SCAN_SUB_ERROR = TOKEN_MAX_VALUE + 1,
    SCAN_SUB_start = TOKEN_MAX_VALUE + 2,
    SCAN_SUB_i_A = TOKEN_MAX_VALUE + 3,
    SCAN_SUB_i_B = TOKEN_MAX_VALUE + 4,
    SCAN_SUB_i_H = TOKEN_MAX_VALUE + 5,
    SCAN_SUB_i_J = TOKEN_MAX_VALUE + 6,
    SCAN_SUB_i_L = TOKEN_MAX_VALUE + 7,
    SCAN_SUB_i_N = TOKEN_MAX_VALUE + 8,
    SCAN_SUB_i_O = TOKEN_MAX_VALUE + 9,
    SCAN_SUB_i_S = TOKEN_MAX_VALUE + 10,
    SCAN_SUB_i_X = TOKEN_MAX_VALUE + 11,
    SCAN_SUB_bif__ = TOKEN_MAX_VALUE + 12,
    SCAN_SUB_d_ = TOKEN_MAX_VALUE + 13,
    SCAN_SUB_decimal_start = TOKEN_MAX_VALUE + 14,
    SCAN_SUB_zero_transition = TOKEN_MAX_VALUE + 15,
    SCAN_SUB_decimal_loop = TOKEN_MAX_VALUE + 16,
    SCAN_SUB_reg_start = TOKEN_MAX_VALUE + 17,
    SCAN_SUB_flag_start = TOKEN_MAX_VALUE + 18,
    SCAN_SUB_colon = TOKEN_MAX_VALUE + 19,
    SCAN_SUB_comment_loop = TOKEN_MAX_VALUE + 20,
    SCAN_SUB_string_loop = TOKEN_MAX_VALUE + 21,
    SCAN_SUB_label_loop = TOKEN_MAX_VALUE + 22,
    SCAN_SUB_org_start = TOKEN_MAX_VALUE + 23,
    SCAN_SUB_hex_start = TOKEN_MAX_VALUE + 24,
    SCAN_SUB_hex_loop = TOKEN_MAX_VALUE + 25,
    SCAN_SUB_flag_loop = TOKEN_MAX_VALUE + 26,
    SCAN_SUB_reg_end = TOKEN_MAX_VALUE + 27,
    SCAN_SUB_esc_sequence = TOKEN_MAX_VALUE + 28,
    SCAN_SUB_esc_hex_start = TOKEN_MAX_VALUE + 29,
    SCAN_SUB_esc_hex_end = TOKEN_MAX_VALUE + 30,
    SCAN_SUB_string_end = TOKEN_MAX_VALUE + 31,
    SCAN_SUB_org_end = TOKEN_MAX_VALUE + 32,
    SCAN_SUB_i_AN = TOKEN_MAX_VALUE + 33,
    SCAN_SUB_i_AD = TOKEN_MAX_VALUE + 34,
    SCAN_SUB_i_AND = TOKEN_MAX_VALUE + 35,
    SCAN_SUB_i_ADD = TOKEN_MAX_VALUE + 36,
    SCAN_SUB_i_BR = TOKEN_MAX_VALUE + 37,
    SCAN_SUB_i_BRC = TOKEN_MAX_VALUE + 38,
    SCAN_SUB_i_HL = TOKEN_MAX_VALUE + 39,
    SCAN_SUB_i_HLT = TOKEN_MAX_VALUE + 40,
    SCAN_SUB_i_JP = TOKEN_MAX_VALUE + 41,
    SCAN_SUB_i_JL = TOKEN_MAX_VALUE + 42,
    SCAN_SUB_i_JPR = TOKEN_MAX_VALUE + 43,
    SCAN_SUB_i_JLR = TOKEN_MAX_VALUE + 44,
    SCAN_SUB_i_LB = TOKEN_MAX_VALUE + 45,
    SCAN_SUB_i_LD = TOKEN_MAX_VALUE + 46,
    SCAN_SUB_i_LBI = TOKEN_MAX_VALUE + 47,
    SCAN_SUB_i_LDR = TOKEN_MAX_VALUE + 48,
    SCAN_SUB_i_NO = TOKEN_MAX_VALUE + 49,
    SCAN_SUB_i_NOP = TOKEN_MAX_VALUE + 50,
    SCAN_SUB_i_OR = TOKEN_MAX_VALUE + 51,
    SCAN_SUB_i_ORR = TOKEN_MAX_VALUE + 52,
    SCAN_SUB_i_SH = TOKEN_MAX_VALUE + 53,
    SCAN_SUB_i_SW = TOKEN_MAX_VALUE + 54,
    SCAN_SUB_i_ST = TOKEN_MAX_VALUE + 55,
    SCAN_SUB_i_SU = TOKEN_MAX_VALUE + 56,
    SCAN_SUB_i_SHL = TOKEN_MAX_VALUE + 57,
    SCAN_SUB_i_SHR = TOKEN_MAX_VALUE + 58,
    SCAN_SUB_i_SWP = TOKEN_MAX_VALUE + 59,
    SCAN_SUB_i_STR = TOKEN_MAX_VALUE + 60,
    SCAN_SUB_i_SUB = TOKEN_MAX_VALUE + 61,
    SCAN_SUB_i_XO = TOKEN_MAX_VALUE + 62,
    SCAN_SUB_i_XOR = TOKEN_MAX_VALUE + 63,
    SCAN_SUB_bif__l = TOKEN_MAX_VALUE + 64,
    SCAN_SUB_bif__s = TOKEN_MAX_VALUE + 65,
    SCAN_SUB_bif__t = TOKEN_MAX_VALUE + 66,
    SCAN_SUB_bif__ld = TOKEN_MAX_VALUE + 67,
    SCAN_SUB_bif__la = TOKEN_MAX_VALUE + 68,
    SCAN_SUB_bif__st = TOKEN_MAX_VALUE + 69,
    SCAN_SUB_bif__to = TOKEN_MAX_VALUE + 70,
    SCAN_SUB_d_w = TOKEN_MAX_VALUE + 71,
    SCAN_SUB_d_a = TOKEN_MAX_VALUE + 72,
    SCAN_SUB_d_g = TOKEN_MAX_VALUE + 73,
    SCAN_SUB_d_h = TOKEN_MAX_VALUE + 74,
    SCAN_SUB_d_wo = TOKEN_MAX_VALUE + 75,
    SCAN_SUB_d_wor = TOKEN_MAX_VALUE + 76,
    SCAN_SUB_d_word = TOKEN_MAX_VALUE + 77,
    SCAN_SUB_d_ar = TOKEN_MAX_VALUE + 78,
    SCAN_SUB_d_arr = TOKEN_MAX_VALUE + 79,
    SCAN_SUB_d_arry = TOKEN_MAX_VALUE + 80,
    SCAN_SUB_d_gl = TOKEN_MAX_VALUE + 81,
    SCAN_SUB_d_glb = TOKEN_MAX_VALUE + 82,
    SCAN_SUB_d_glbl = TOKEN_MAX_VALUE + 83,
    SCAN_SUB_d_he = TOKEN_MAX_VALUE + 84,
    SCAN_SUB_d_hea = TOKEN_MAX_VALUE + 85,
    SCAN_SUB_d_heap = TOKEN_MAX_VALUE + 86,
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
	switch (m_rawState) {
        case SCAN_SUB_start:
            if (IS('A')) {nextState = SCAN_SUB_i_A; break;}
            else if (IS('B')) {nextState = SCAN_SUB_i_B; break;}
            else if (IS('H')) {nextState = SCAN_SUB_i_H; break;}
            else if (IS('J')) {nextState = SCAN_SUB_i_J; break;}
            else if (IS('L')) {nextState = SCAN_SUB_i_L; break;}
            else if (IS('N')) {nextState = SCAN_SUB_i_N; break;}
            else if (IS('O')) {nextState = SCAN_SUB_i_O; break;}
            else if (IS('S')) {nextState = SCAN_SUB_i_S; break;}
            else if (IS('X')) {nextState = SCAN_SUB_i_X; break;}
            else if (IS('_')) {nextState = SCAN_SUB_bif__; break;}
            else if (IS('.')) {nextState = SCAN_SUB_d_; break;}
            else if (IS('+')) {nextState = SCAN_SUB_decimal_start; break;}
            else if (IS('-')) {nextState = SCAN_SUB_decimal_start; break;}
            else if (IS('0')) {nextState = SCAN_SUB_zero_transition; break;}
            else if (IN('1','9')) {nextState = SCAN_SUB_decimal_loop; break;}
            else if (IS('$')) {nextState = SCAN_SUB_reg_start; break;}
            else if (IS('%')) {nextState = SCAN_SUB_flag_start; break;}
            else if (IS(':')) {nextState = SCAN_SUB_colon; break;}
            else if (IS(';')) {nextState = SCAN_SUB_comment_loop; break;}
            else if (IS('"')) {nextState = SCAN_SUB_string_loop; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            else if (IS('\t')) {nextState = SCAN_SUB_start; break;}
            else if (IS('\r')) {nextState = SCAN_SUB_start; break;}
            else if (IS('\n')) {nextState = SCAN_SUB_start; break;}
            else if (IS(' ')) {nextState = SCAN_SUB_start; break;}
            else if (IS('#')) {nextState = SCAN_SUB_org_start; break;}
            else if (EOF_CHAR) {nextState = TOKEN_EOF; break;}
            break;
        case SCAN_SUB_decimal_start:
            if (IN('0','9')) {nextState = SCAN_SUB_decimal_loop; break;}
            break;
        case SCAN_SUB_zero_transition:
            if (IS('x')) {nextState = SCAN_SUB_hex_start; break;}
            else if (IN('0','9')) {nextState = SCAN_SUB_decimal_loop; break;}
            nextState = TOKEN_IMMEDIATE;
            break;
        case SCAN_SUB_hex_start:
            if (HEX_CHAR) {nextState = SCAN_SUB_hex_loop; break;}
            break;
        case SCAN_SUB_flag_start:
            if (IS('a')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('s')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('n')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('z')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('p')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('c')) {nextState = SCAN_SUB_flag_loop; break;}
            break;
        case SCAN_SUB_reg_start:
            if (IN('0','7')) {nextState = SCAN_SUB_reg_end; break;}
            break;
        case SCAN_SUB_reg_end:
            if (IN('0','9')) {nextState = SCAN_SUB_ERROR; break;}
            nextState = TOKEN_REGISTER;
            break;
        case SCAN_SUB_colon:
            nextState = TOKEN_COLON;
            break;
        case SCAN_SUB_esc_sequence:
            if (IS('n')) {nextState = SCAN_SUB_string_loop; break;}
            else if (IS('r')) {nextState = SCAN_SUB_string_loop; break;}
            else if (IS('t')) {nextState = SCAN_SUB_string_loop; break;}
            else if (IS('0')) {nextState = SCAN_SUB_string_loop; break;}
            else if (IS('\\')) {nextState = SCAN_SUB_string_loop; break;}
            else if (IS('"')) {nextState = SCAN_SUB_string_loop; break;}
            else if (IS('x')) {nextState = SCAN_SUB_esc_hex_start; break;}
            break;
        case SCAN_SUB_esc_hex_start:
            if (HEX_CHAR) {nextState = SCAN_SUB_esc_hex_end; break;}
            break;
        case SCAN_SUB_esc_hex_end:
            if (HEX_CHAR) {nextState = SCAN_SUB_string_loop; break;}
            break;
        case SCAN_SUB_string_end:
            nextState = TOKEN_STRING;
            break;
        case SCAN_SUB_org_start:
            if (IS('r')) {nextState = SCAN_SUB_org_end; break;}
            else if (IS('d')) {nextState = SCAN_SUB_org_end; break;}
            else if (IS('b')) {nextState = SCAN_SUB_org_end; break;}
            break;
        case SCAN_SUB_org_end:
            nextState = TOKEN_SECTION;
            break;
        case SCAN_SUB_label_loop:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_decimal_loop:
            if (IN('0','9')) {nextState = SCAN_SUB_decimal_loop; break;}
            nextState = TOKEN_IMMEDIATE;
            break;
        case SCAN_SUB_hex_loop:
            if (HEX_CHAR) {nextState = SCAN_SUB_hex_loop; break;}
            nextState = TOKEN_IMMEDIATE;
            break;
        case SCAN_SUB_flag_loop:
            if (IS('a')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('s')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('n')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('z')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('p')) {nextState = SCAN_SUB_flag_loop; break;}
            else if (IS('c')) {nextState = SCAN_SUB_flag_loop; break;}
            nextState = TOKEN_FLAG;
            break;
        case SCAN_SUB_comment_loop:
            if (IS('\n')) {nextState = TOKEN_COMMENT; break;}
            else if (IS('\0')) {nextState = TOKEN_COMMENT; break;}
            nextState = SCAN_SUB_comment_loop;
            break;
        case SCAN_SUB_string_loop:
            if (IS('\\')) {nextState = SCAN_SUB_esc_sequence; break;}
            else if (IS('"')) {nextState = SCAN_SUB_string_end; break;}
            nextState = SCAN_SUB_string_loop;
            break;
        case SCAN_SUB_i_A:
            if (IS('N')) {nextState = SCAN_SUB_i_AN; break;}
            else if (IS('D')) {nextState = SCAN_SUB_i_AD; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_AN:
            if (IS('D')) {nextState = SCAN_SUB_i_AND; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_AD:
            if (IS('D')) {nextState = SCAN_SUB_i_ADD; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_B:
            if (IS('R')) {nextState = SCAN_SUB_i_BR; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_BR:
            if (IS('C')) {nextState = SCAN_SUB_i_BRC; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_H:
            if (IS('L')) {nextState = SCAN_SUB_i_HL; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_HL:
            if (IS('T')) {nextState = SCAN_SUB_i_HLT; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_J:
            if (IS('P')) {nextState = SCAN_SUB_i_JP; break;}
            else if (IS('L')) {nextState = SCAN_SUB_i_JL; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_JP:
            if (IS('R')) {nextState = SCAN_SUB_i_JPR; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_JL:
            if (IS('R')) {nextState = SCAN_SUB_i_JLR; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_L:
            if (IS('B')) {nextState = SCAN_SUB_i_LB; break;}
            else if (IS('D')) {nextState = SCAN_SUB_i_LD; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_LB:
            if (IS('I')) {nextState = SCAN_SUB_i_LBI; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_LD:
            if (IS('R')) {nextState = SCAN_SUB_i_LDR; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_N:
            if (IS('O')) {nextState = SCAN_SUB_i_NO; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_NO:
            if (IS('P')) {nextState = SCAN_SUB_i_NOP; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_O:
            if (IS('R')) {nextState = SCAN_SUB_i_OR; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_OR:
            if (IS('R')) {nextState = SCAN_SUB_i_ORR; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_S:
            if (IS('H')) {nextState = SCAN_SUB_i_SH; break;}
            else if (IS('W')) {nextState = SCAN_SUB_i_SW; break;}
            else if (IS('T')) {nextState = SCAN_SUB_i_ST; break;}
            else if (IS('U')) {nextState = SCAN_SUB_i_SU; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_SH:
            if (IS('L')) {nextState = SCAN_SUB_i_SHL; break;}
            else if (IS('R')) {nextState = SCAN_SUB_i_SHR; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_SW:
            if (IS('P')) {nextState = SCAN_SUB_i_SWP; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_ST:
            if (IS('R')) {nextState = SCAN_SUB_i_STR; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_SU:
            if (IS('B')) {nextState = SCAN_SUB_i_SUB; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_X:
            if (IS('O')) {nextState = SCAN_SUB_i_XO; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_i_XO:
            if (IS('R')) {nextState = SCAN_SUB_i_XOR; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_bif__:
            if (IS('l')) {nextState = SCAN_SUB_bif__l; break;}
            else if (IS('s')) {nextState = SCAN_SUB_bif__s; break;}
            else if (IS('t')) {nextState = SCAN_SUB_bif__t; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_bif__l:
            if (IS('d')) {nextState = SCAN_SUB_bif__ld; break;}
            else if (IS('a')) {nextState = SCAN_SUB_bif__la; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_bif__s:
            if (IS('t')) {nextState = SCAN_SUB_bif__st; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_bif__t:
            if (IS('o')) {nextState = SCAN_SUB_bif__to; break;}
            else if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LABEL;
            break;
        case SCAN_SUB_d_:
            if (IS('w')) {nextState = SCAN_SUB_d_w; break;}
            else if (IS('a')) {nextState = SCAN_SUB_d_a; break;}
            else if (IS('g')) {nextState = SCAN_SUB_d_g; break;}
            else if (IS('h')) {nextState = SCAN_SUB_d_h; break;}
            break;
        case SCAN_SUB_d_w:
            if (IS('o')) {nextState = SCAN_SUB_d_wo; break;}
            break;
        case SCAN_SUB_d_wo:
            if (IS('r')) {nextState = SCAN_SUB_d_wor; break;}
            break;
        case SCAN_SUB_d_wor:
            if (IS('d')) {nextState = SCAN_SUB_d_word; break;}
            break;
        case SCAN_SUB_d_a:
            if (IS('r')) {nextState = SCAN_SUB_d_ar; break;}
            break;
        case SCAN_SUB_d_ar:
            if (IS('r')) {nextState = SCAN_SUB_d_arr; break;}
            break;
        case SCAN_SUB_d_arr:
            if (IS('y')) {nextState = SCAN_SUB_d_arry; break;}
            break;
        case SCAN_SUB_d_g:
            if (IS('l')) {nextState = SCAN_SUB_d_gl; break;}
            break;
        case SCAN_SUB_d_gl:
            if (IS('b')) {nextState = SCAN_SUB_d_glb; break;}
            break;
        case SCAN_SUB_d_glb:
            if (IS('l')) {nextState = SCAN_SUB_d_glbl; break;}
            break;
        case SCAN_SUB_d_h:
            if (IS('e')) {nextState = SCAN_SUB_d_he; break;}
            break;
        case SCAN_SUB_d_he:
            if (IS('a')) {nextState = SCAN_SUB_d_hea; break;}
            break;
        case SCAN_SUB_d_hea:
            if (IS('p')) {nextState = SCAN_SUB_d_heap; break;}
            break;
        case SCAN_SUB_i_AND:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_AND;
            break;
        case SCAN_SUB_i_ADD:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_ADD;
            break;
        case SCAN_SUB_i_BRC:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_BRC;
            break;
        case SCAN_SUB_i_HLT:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_HLT;
            break;
        case SCAN_SUB_i_JPR:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_JPR;
            break;
        case SCAN_SUB_i_JLR:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_JLR;
            break;
        case SCAN_SUB_i_LBI:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LBI;
            break;
        case SCAN_SUB_i_LDR:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LDR;
            break;
        case SCAN_SUB_i_NOP:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_NOP;
            break;
        case SCAN_SUB_i_ORR:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_ORR;
            break;
        case SCAN_SUB_i_SHL:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_SHL;
            break;
        case SCAN_SUB_i_SHR:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_SHR;
            break;
        case SCAN_SUB_i_SWP:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_SWP;
            break;
        case SCAN_SUB_i_STR:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_STR;
            break;
        case SCAN_SUB_i_SUB:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_SUB;
            break;
        case SCAN_SUB_i_XOR:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_XOR;
            break;
        case SCAN_SUB_bif__ld:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LD;
            break;
        case SCAN_SUB_bif__la:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_LA;
            break;
        case SCAN_SUB_bif__st:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_ST;
            break;
        case SCAN_SUB_bif__to:
            if (LBL_CHAR) {nextState = SCAN_SUB_label_loop; break;}
            nextState = TOKEN_TO;
            break;
        case SCAN_SUB_d_word:
            nextState = TOKEN_WORD;
            break;
        case SCAN_SUB_d_arry:
            nextState = TOKEN_ARRY;
            break;
        case SCAN_SUB_d_glbl:
            nextState = TOKEN_GLBL;
            break;
        case SCAN_SUB_d_heap:
            nextState = TOKEN_HEAP;
            break;
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
