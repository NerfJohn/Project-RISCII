/*
 * StepParseCli.h: State/step to process command line inputs into data model.
 */

#ifndef STATE_STEPPARSECLI_H_
#define STATE_STEPPARSECLI_H_

#include "Domain/DataModel_t.h"

// TODO
void StepParseCli_execute(DataModel_t& model,
		                  int const argc,
						  char const* const argv[]);

#endif /* STATE_STEPPARSECLI_H_ */
