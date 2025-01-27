/*
 * BuildType_e.h: ID for elements used to build nodes. Extends LexToken_e.
 */

#ifndef DOMAIN_BUILDTYPE_E_H_
#define DOMAIN_BUILDTYPE_E_H_

#include "Domain/LexToken_e.h"

// TODO
typedef enum {
	// Real values.
	BUILD_FILE = TOKEN_LEX_MAX + 1,    // FileNode
	BUILD_TEXT,                        // TextNode
	BUILD_INC,                         // IncNode
} BuildType_e;

#endif /* DOMAIN_BUILDTYPE_E_H_ */
