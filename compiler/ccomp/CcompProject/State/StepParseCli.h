/*
 * StepParseCli.h: Step to process command line inputs into data model.
 */

#ifndef STATE_STEPPARSECLI_H_
#define STATE_STEPPARSECLI_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Check/parses arguments from command line into data model.
 *
 * State parses the raw command line input, recording files and flags found in
 * the data model. Also performs error checking and response handling of certain
 * flags (e.g. -h flag).
 *
 * @param model data model of the entire program
 * @param argc  number of command line arguments
 * @param argv  array of c-string command line arguments
 */
void StepParseCli_execute(DataModel_t& model, int argc, char* argv[]);

#endif /* STATE_STEPPARSECLI_H_ */
