/*
 * ParseCliStep.h
 *
 * "Step of assembly process focused on parsing the command line inputs"
 */

#ifndef SRC_APPLAYER_PARSECLISTEP_H_
#define SRC_APPLAYER_PARSECLISTEP_H_

#include "DomainLayer/DataModel_t.h"

/*
 * Parses CLI input into data model, implementing select flags immediately.
 *
 * Parses files, flags, and flag arguments into the data model. Once parsed,
 * it processes select flags/options (namely those that should be handled prior
 * to parsing the input files).
 *
 * @param argc number of CLI tokens in argv
 * @param argv CLI tokens passed to assembler
 * @param model data model of the program
 */
void ParseCliStep_parseCli(int argc, char* argv[], DataModel_t& model);

#endif /* SRC_APPLAYER_PARSECLISTEP_H_ */
