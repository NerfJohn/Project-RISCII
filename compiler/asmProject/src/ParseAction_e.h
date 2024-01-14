/*
 * ParseAction_e.h
 *
 *  Created on: Jan 13, 2024
 *      Author: John
 */

#ifndef SRC_PARSEACTION_E_H_
#define SRC_PARSEACTION_E_H_

#include "SyntaxToken_e.h"

// TODO- defines actions during parsing- extends SytaxToken_e
typedef enum ParseAction_e {
	// Core Values.
	ACTION_INSTR_RRR  = TOKEN_VAL_MAX + 1,	// Instruction formats  (433123)
	ACTION_INSTR_RRI4 = TOKEN_VAL_MAX + 2,	//						(433114)
	ACTION_INSTR_RRI5 = TOKEN_VAL_MAX + 3,	//						(43315)
	ACTION_INSTR_RRI6 = TOKEN_VAL_MAX + 4,	//						(4336)
	ACTION_INSTR_RI5  = TOKEN_VAL_MAX + 5,	//						(43315)
	ACTION_INSTR_RI8  = TOKEN_VAL_MAX + 6,	//						(4318)
	ACTION_INSTR_I8   = TOKEN_VAL_MAX + 7,	//						(448)
	ACTION_INSTR_CMD  = TOKEN_VAL_MAX + 8,  // ~Instruction formats	(4'12')
	ACTION_MACRO_LD   = TOKEN_VAL_MAX + 9,  // Load macro
	ACTION_MACRO_ST   = TOKEN_VAL_MAX + 10, // Store macro
	ACTION_MACRO_DS   = TOKEN_VAL_MAX + 11, // Data Space macro
	ACTION_MACRO_TO   = TOKEN_VAL_MAX + 12, // Jump macro
	ACTION_WORD       = TOKEN_VAL_MAX + 13, // .word directive
	ACTION_ARRY       = TOKEN_VAL_MAX + 14, // .arry directive
	ACTION_ADDR       = TOKEN_VAL_MAX + 15, // .addr directive
	ACTION_LABEL_DECL = TOKEN_VAL_MAX + 16, // Label declaration

	// Special Values.
	ACTION_INVALID = TOKEN_VAL_MAX + 17,	// Invalid/unrecognized action
	ACTION_VAL_MIN = ACTION_INSTR_RRR,		// Lowest parse action
	ACTION_VAL_MAX = ACTION_INVALID,		// Highest parse action
} ParseAction_e;

#endif /* SRC_PARSEACTION_E_H_ */
