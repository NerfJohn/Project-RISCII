/*
 * ReadFilesStep.h
 *
 * "Step of assembly process focused on reading each input file"
 */

#ifndef SRC_APPLAYER_READFILESSTEP_H_
#define SRC_APPLAYER_READFILESSTEP_H_

#include "DomainLayer/DataModel_t.h"

/*
 * Reads input files specified in model. Adds parsed/checked items to model.
 *
 * Read step reads, parses, and analyzes each file- adding the consequent build
 * items to the data model. Step will print/terminate the program if errors are
 * found (printing/potentially terminating after each file).
 *
 * @param model data model of the program
 */
void ReadFilesStep_readFiles(DataModel_t* model);

#endif /* SRC_APPLAYER_READFILESSTEP_H_ */
