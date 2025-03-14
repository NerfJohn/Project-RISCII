/*
 * StepCreateFile.h: Step to create new pre-processed file.
 */

#ifndef STATE_STEPCREATEFILE_H_
#define STATE_STEPCREATEFILE_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Translates source file into (created) pre-processed file.
 *
 * Checks and translates nodes specific to translation unit- including
 * definitions/conditions/etc. Given file used as basis for output file path.
 * Terminates program for errors.
 *
 * @param model data model of the entire program
 * @param file  source file/path to translate into an output file
 */
void StepCreateFile_execute(DataModel_t& model, std::string& file);

#endif /* STATE_STEPCREATEFILE_H_ */
