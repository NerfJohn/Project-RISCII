/*
 * SubStepParseTkns.h: Substep to parse tokens into series of tokens.
 */

#ifndef STATE_SUBSTEPPARSETKNS_H_
#define STATE_SUBSTEPPARSETKNS_H_

#include <queue>
#include "Domain/DataModel_t.h"
#include "Ds/AAstNode.h"
#include "Ds/ItemToken.h"

// TODO
/*
 * Executes sub-process to parse tokens into an ordered series of nodes.
 *
 * Parses the token list into a list of nodes, conducting error checking and
 * assertions. Being a substep, this function returns instead of terminating.
 *
 * @param model shared data of the entire program
 * @param tkns  ordered list of tokens to parse into nodes
 * @param nodes ordered list of nodes to populate based on token list
 */
void SubStepParseTkns_execute(DataModel_t&            model,
		                      std::queue<ItemToken*>& tkns,
						      AAstNode*&              root);

#endif /* STATE_SUBSTEPPARSETKNS_H_ */
