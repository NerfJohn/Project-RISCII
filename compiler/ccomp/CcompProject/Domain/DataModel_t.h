/*
 * DataModel_t.h: Critical data passed between states/steps of the program.
 */

#ifndef DOMAIN_DATAMODEL_T_H_
#define DOMAIN_DATAMODEL_T_H_

#include <cstdint>
#include <string>
#include <vector>
#include "Domain/RetCode_e.h"

/*
 * @brief Data passed in-between states/pieces of the entire program.
 *
 * "Model" of the program with respect to data- any info shared between larger
 * parts of the program is saved and shared via this struct. It is intended for
 * the program to only ever have one, shared instance of this type.
 */
typedef struct {
	// General Summary/Progress.
	uint32_t  m_numErrs;                         // # of errors found
	uint32_t  m_numWarns;                        // # of warnings found
	RetCode_e m_exitCode;                        // exit code to exit with

	// Parsed Cli Results.
	std::vector<std::string> m_files;            // input files
	bool                     m_doOpt;            // optimize option
} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
