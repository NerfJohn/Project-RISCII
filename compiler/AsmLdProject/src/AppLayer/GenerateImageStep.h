/*
 * GenerateImageStep.h
 *
 * "Translates program into binary image file"
 */

#ifndef SRC_APPLAYER_GENERATEIMAGESTEP_H_
#define SRC_APPLAYER_GENERATEIMAGESTEP_H_

#include "DomainLayer/DataModel_t.h"

/*
 * Translates model's items into binary image- creating the file in the process.
 *
 * First translates and organizes the items into binary values. Once done, it
 * opens/creates the output file and populates it with the translated values.
 * This function implements a "step/state" of the assembler.
 *
 * @param model data model of the program
 */
void GenerateImageStep_generateImage(DataModel_t& model);

#endif /* SRC_APPLAYER_GENERATEIMAGESTEP_H_ */
