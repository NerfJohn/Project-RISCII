/*
 * StepLexFile.h: Step to lex file into series of tokens (into model).
 */

#ifndef STATE_STEPLEXFILE_H_
#define STATE_STEPLEXFILE_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Lexes given file into tokens- saving the list to the data model.
 *
 * State lexes raw file input, recording resulting tokens (in order) to a list
 * within the data model. Handles error checking of file/lex operations.
 *
 * @param model data model of the program
 * @param fname name of the file to lex
 */
void StepLexFile_execute(DataModel_t& model, std::string const& fname);

#endif /* STATE_STEPLEXFILE_H_ */
