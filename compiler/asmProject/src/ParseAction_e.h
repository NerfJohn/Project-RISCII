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
	ACTION_FOO = TOKEN_MAX_VALUE + 1,

	// Special values.
	ACTION_INVALID = TOKEN_MAX_VALUE + 2,
	ACTION_MAX_VALUE = ACTION_FOO,
} ParseAction_e;


#endif /* SRC_PARSEACTION_E_H_ */
