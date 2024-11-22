/*
 * BuildStack_t.h: Stack of (pointers to) items used for building nodes.
 */

#ifndef DOMAIN_BUILDSTACK_T_H_
#define DOMAIN_BUILDSTACK_T_H_

#include <memory>
#include <stack>
#include "Ds/ItemToken.h"

/*
 * List of allocated tokens used to build a node.
 *
 * Effectively just a wrapper to keep type names shorter. It is intended to be
 * used to handle the accumulation and passing of ItemToken objects from the
 * parser to constructed nodes.
 */
typedef std::stack<std::shared_ptr<ItemToken>> BuildStack_t;

#endif /* DOMAIN_BUILDSTACK_T_H_ */
