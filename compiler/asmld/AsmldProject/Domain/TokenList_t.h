/*
 * TokenList_t.h: List of (pointers to) tokens lexed from a file.
 */

#ifndef DOMAIN_TOKENLIST_T_H_
#define DOMAIN_TOKENLIST_T_H_

#include <memory>
#include <queue>
#include "Ds/ItemToken.h"

/*
 * List of allocated tokens used to build nodes.
 *
 * Effectively just a wrapper to keep type names more readable. It is intended
 * to be used to transfer lexed tokens from the lexer to the parser, which
 * re-assigns "ownership" of the tokens to nodes (and core data structure).
 */
typedef std::queue<std::shared_ptr<ItemToken>> TokenList_t;

#endif /* DOMAIN_TOKENLIST_T_H_ */
