/*
 * StepReadFiles.h: State/step to parse and analyze the input files.
 */

#ifndef STATE_STEPREADFILES_H_
#define STATE_STEPREADFILES_H_

#include "Domain/DataModel_t.h"

/*
 * Executes process to analyze each input file in the model, saving the results.
 *
 * State iterates through the input files (saved in the model) and reads them,
 * analyzing them for errors/data. Once finished, it adds the parsed data to the
 * model. Terminates program upon found errors.
 *
 * @param model shared data of the entire program
 */
void StepReadFiles_execute(DataModel_t& model);

#endif /* STATE_STEPREADFILES_H_ */
