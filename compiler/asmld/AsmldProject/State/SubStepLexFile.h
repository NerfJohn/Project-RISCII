/*
 * SubStepLexFile.h: Substep to lex file as a series of tokens.
 */

#ifndef STATE_SUBSTEPLEXFILE_H_
#define STATE_SUBSTEPLEXFILE_H_

#include <vector>
#include "Domain/DataModel_t.h"
#include "Domain/FileToken_t.h"
#include "Domain/RetErr_e.h"

/*
 * Executes sub-process to lex opened file into a series of file tokens.
 *
 * Assuming File::inst() represents the opened file to lex, conducts byte for
 * byte lexing of file, including error checking + debug logging. Being a sub
 * step, this function returns a result regarding errors (vs terminating).
 *
 * @param model shared data of the entire program
 * @param tkns  ordered list of tokens to populate based on opened file
 * @return      RET_ERR_NONE is successful, RET_ERR_ERROR otherwise
 */
RetErr_e SubStepLexFile_execute(DataModel_t& model,
		                        std::vector<FileToken_t>& tkns);

#endif /* STATE_SUBSTEPLEXFILE_H_ */
