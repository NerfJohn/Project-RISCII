/*
 * NodeList_t.h: List of (pointers to) nodes creating the core data structure.
 */

#ifndef DOMAIN_NODELIST_T_H_
#define DOMAIN_NODELIST_T_H_

#include <memory>
#include <vector>
#include "Ds/AAsmNode.h"

/*
 * List of allocated nodes representing (a part of) the core data structure.
 *
 * Effectively just a wrapper to keep type names more readable, though does
 * provide a convenient way to hold pieces, if not all, of the
 * assembler/linker's core data structure.
 */
typedef std::vector<std::shared_ptr<AAsmNode>> NodeList_t;

#endif /* DOMAIN_NODELIST_T_H_ */
