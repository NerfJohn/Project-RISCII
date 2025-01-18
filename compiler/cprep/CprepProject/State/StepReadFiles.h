/*
 * StepReadFiles.h: State/step to parse and analyze the input/include files.
 */

#ifndef STATE_STEPREADFILES_H_
#define STATE_STEPREADFILES_H_

#include "Domain/DataModel_t.h"

/*
 * Executes process to analyze an input + include files, saving the results.
 *
 * State analyzes a given input files, recursively analyzing any included files
 * as well. Files are saved as abstract syntax trees (ASTs) for later use.
 * Terminates program upon found errors.
 *
 * @param model shared data of the entire program
 * @param fname path/name of the original input file
 */
void StepReadFiles_execute(DataModel_t& model, std::string const& fname);

#endif /* STATE_STEPREADFILES_H_ */
