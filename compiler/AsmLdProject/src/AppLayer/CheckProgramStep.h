/*
 * CheckProgramStep.h
 *
 * "Step of checking/analyzing parsed program as one unit"
 */

#ifndef SRC_APPLAYER_CHECKPROGRAMSTEP_H_
#define SRC_APPLAYER_CHECKPROGRAMSTEP_H_

#include "DomainLayer/DataModel_t.h"

/*
 * Checks parsed build items for program-level feasibility.
 *
 * Runs checks/analysis over all build items and program data to ensure program
 * can be built correctly. These checks are done at the program level and are
 * distinct from the file-level checks/analysis done during parsing.
 *
 * @param model data model of the (assembler) program
 */
void CheckProgramStep_checkProgram(DataModel_t* model);

#endif /* SRC_APPLAYER_CHECKPROGRAMSTEP_H_ */
