/*
 * StepDoProcedure.h: Step to conduct main procedure/calls of program.
 */

#ifndef STATE_STEPDOPROCEDURE_H_
#define STATE_STEPDOPROCEDURE_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Runs main series of command line calls to compile program.
 *
 * Incorporates logic to build/run commands, as well as the logic surrounding
 * the calling behavior. Terminates if errors are found.
 *
 * @param model data model of the entire program
 */
void StepDoProcedure_execute(DataModel_t& model);

#endif /* STATE_STEPDOPROCEDURE_H_ */
