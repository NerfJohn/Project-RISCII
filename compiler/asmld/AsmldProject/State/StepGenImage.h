/*
 * StepGenImage.h: Step/process to generate binary image from data model.
 */

#ifndef STATE_STEPGENIMAGE_H_
#define STATE_STEPGENIMAGE_H_

#include "Domain/DataModel_t.h"

/*
 * Executes process to generate binary image from model.
 *
 * Assembles and links nodes' data into suitable binary image for the target.
 * Write is only carried out if the image can be created without errors (eg
 * program can fir within image). Terminates program upon errors found.
 *
 * @param model shared data of the entire program
 */
void StepGenImage_execute(DataModel_t& model);

#endif /* STATE_STEPGENIMAGE_H_ */
