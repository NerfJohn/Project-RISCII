/*
 * StepParseCli.h: Step to process command line inputs into program.
 */

#ifndef STATE_STEPPARSECLI_H_
#define STATE_STEPPARSECLI_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Executes process to check/parse command line inputs into model.
 *
 * Handles each input flag/file left to right- recording results to data model.
 * Terminates with error message if errors are found.
 *
 * @param model data model of the entire program
 * @param argc  number of command line arguments
 * @param argv  array of c-string command line arguments
 */
void StepParseCli_execute(DataModel_t&      model,
		                  int const         argc,
						  char const* const argv[]);

#endif /* STATE_STEPPARSECLI_H_ */
