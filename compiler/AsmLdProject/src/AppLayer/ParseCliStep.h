/*
 * ParseCliStep.h
 *
 * "Step of assembly process focused on parsing the command line inputs"
 */

#ifndef SRC_APPLAYER_PARSECLISTEP_H_
#define SRC_APPLAYER_PARSECLISTEP_H_

#include "DomainLayer/DataModel_t.h"

/*
 * Parses options and filename inputs into the data model.
 *
 * At the moment, the parser assumes all inputs (except for the assembler
 * executable name) are input filenames. It simply copies each name into the
 * model's input filename location.
 *
 * @param argc the number of elements in argv
 * @param argv the tokens passed to the assembler via the command line
 * @param model the program's data model
 */
void ParseCliStep_parseCli(int argc, char* argv[], DataModel_t* model);

#endif /* SRC_APPLAYER_PARSECLISTEP_H_ */
