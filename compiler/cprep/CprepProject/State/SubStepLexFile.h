/*
 * SubStepLexFile.h: Substep to lex opened file into list of tokens.
 */

#ifndef STATE_SUBSTEPLEXFILE_H_
#define STATE_SUBSTEPLEXFILE_H_

#include <queue>
#include "Common/Ds/LexToken.h"
#include "Domain/DataModel_t.h"

/*
 * @brief Lexes opened file into list of tokens. Does not terminate on error.
 *
 * State lexes file opened with Common/Device/File. Creates tokens, adding them
 * to the given list (passing ownership). Performs error checking, but relies on
 * caller to terminate program.
 *
 * @param model data model of the entire program
 * @param tkns  list to populate with tokens (in order of appearance)
 */
void SubStepLexFile_execute(DataModel_t& model, std::queue<LexToken*>& tkns);

#endif /* STATE_SUBSTEPLEXFILE_H_ */
