/*
 * SubStepParseTkns.h: Substep to parse tokens into series of tokens.
 */

#ifndef STATE_SUBSTEPPARSETKNS_H_
#define STATE_SUBSTEPPARSETKNS_H_

#include "Domain/DataModel_t.h"
#include "Domain/NodeList_t.h"
#include "Domain/RetErr_e.h"
#include "Domain/TokenList_t.h"

/*
 * Executes sub-process to parse tokens into an ordered series of nodes.
 *
 * Parses the token list into a list of nodes, conducting error checking and
 * assertions. Being a substep, this function returns a result regarding errors
 * (vs terminating).
 *
 * @param model shared data of the entire program
 * @param tkns  ordered list of tokens to parse into nodes
 * @param nodes ordered list of nodes to populate based on token list
 * @return      RET_ERR_NONE if successful, RET_ERR_ERROR otherwise
 */
RetErr_e SubStepParseTkns_execute(DataModel_t& model,
		                          TokenList_t& tkns,
								  NodeList_t& nodes);

#endif /* STATE_SUBSTEPPARSETKNS_H_ */
