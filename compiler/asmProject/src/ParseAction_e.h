/*
 * ParseAction_e.h
 *
 *  Created on: Feb 18, 2024
 *      Author: John
 */

#ifndef SRC_PARSEACTION_E_H_
#define SRC_PARSEACTION_E_H_

#include "SyntaxToken_e.h"

// TODO- enum representing "action" steps to build language structures.
typedef enum {
	// Core values.
	ACTION_INSTR     = TOKEN_MAX_VALUE + 1,
	ACTION_FUNCTION  = TOKEN_MAX_VALUE + 2,
	ACTION_DATA      = TOKEN_MAX_VALUE + 3,
	ACTION_DIRECTIVE = TOKEN_MAX_VALUE + 4,
	ACTION_LABEL     = TOKEN_MAX_VALUE + 5,

	// Special values.
	ACTION_INVALID = TOKEN_MAX_VALUE + 6,
	ACTION_MAX_VALUE = ACTION_INVALID,
} ParseAction_e;


#endif /* SRC_PARSEACTION_E_H_ */
