/*
 * StepParseFile.h: Step to parse tokens into AST (saved in model).
 */

#ifndef STATE_STEPPARSEFILE_H_
#define STATE_STEPPARSEFILE_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Parses model's tokens into AST, saving it to the model.
 *
 * State converts model's tokens into an AST- potentially modifying both
 * elements. Handles error checking of parsing operations.
 *
 * @param model data model of the program
 */
void StepParseFile_execute(DataModel_t& model);

#endif /* STATE_STEPPARSEFILE_H_ */
