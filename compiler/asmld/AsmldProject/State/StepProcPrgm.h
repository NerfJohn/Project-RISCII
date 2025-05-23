/*
 * StepProcPrgm.h: State/step to process the program between input/output.
 */

#ifndef STATE_STEPPROCPRGM_H_
#define STATE_STEPPROCPRGM_H_

#include "Domain/DataModel_t.h"

/*
 * Executes process to process overall program in model.
 *
 * State completes analysis/parsing of the program, followed by applying any
 * requested modifications (e.g. dead code elimination). Terminates program
 * upon found errors.
 */
void StepProcPrgm_execute(DataModel_t& model);

#endif /* STATE_STEPPROCPRGM_H_ */
