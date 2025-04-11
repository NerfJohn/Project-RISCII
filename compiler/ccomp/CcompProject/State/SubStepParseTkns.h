/*
 * SubStepParseTkns.h: Substep to parsed tokens (of still opened file).
 */

#ifndef STATE_SUBSTEPPARSETKNS_H_
#define STATE_SUBSTEPPARSETKNS_H_

#include <queue>
#include "Common/Ds/LexToken.h"
#include "Domain/DataModel_t.h"
#include "Ds/IAstNode.h"

/*
 * @brief Parses tokens into AST. Does not terminate on error.
 *
 * Creates nodes/AST, passing ownership upon return. For debug reasons,
 * requires file used to generate lex tokens is opened with Common/Device/File.
 * Performs error checking, but relies on caller to terminate program.
 *
 * @param model data model of the entire program
 * @param tkns  list of tokens to parse into nodes
 * @return      top node of AST
 */
IAstNode* SubStepParseTkns_execute(DataModel_t&                 model,
		                           std::queue<LexToken*> const& tkns);

#endif /* STATE_SUBSTEPPARSETKNS_H_ */
