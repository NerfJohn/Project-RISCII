/*
 * StepReadFile.h: Step to parse file into program as AST.
 */

#ifndef STATE_STEPREADFILE_H_
#define STATE_STEPREADFILE_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Parse source file (and associated include files) into ASTs.
 *
 * Assumes Common/Device/File is closed. Populates model with source's AST
 * directly. Terminates program upon found errors.
 *
 * @param model   data model of the entire program
 * @param fname   name of the file to parse
 */
void StepReadFiles_execute(DataModel_t& model, std::string const& fname);

#endif /* STATE_STEPREADFILE_H_ */
