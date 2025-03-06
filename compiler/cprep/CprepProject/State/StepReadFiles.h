/*
 * StepReadFiles.h: Step to process source file + associated include files.
 */

#ifndef STATE_STEPREADFILES_H_
#define STATE_STEPREADFILES_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Parse source file (and associated include files) into ASTs.
 *
 * Assumes Common/Device/File is closed. Populates model with source's AST
 * directly (and include ASTs indirectly via SymTable). Terminates program
 * upon found errors.
 *
 * @param model   data model of the entire program
 * @param srcName name of the source file to parse
 */
void StepReadFiles_execute(DataModel_t& model, std::string const& srcName);

#endif /* STATE_STEPREADFILES_H_ */
