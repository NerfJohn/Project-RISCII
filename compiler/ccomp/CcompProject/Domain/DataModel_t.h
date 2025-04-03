/*
 * DataModel_t.h: Critical data passed between states/steps of the program.
 */

#ifndef DOMAIN_DATAMODEL_T_H_
#define DOMAIN_DATAMODEL_T_H_

#include <string>
#include <vector>
#include "Common/Domain/PrgmInfo_t.h"

/*
 * @brief Data passed in-between states/pieces of the entire program.
 *
 * "Model" of the program with respect to data- any info shared between larger
 * parts of the program is saved and shared via this struct. It is intended for
 * the program to only ever have one, shared instance of this type.
 */
typedef struct {
	// General Summary/Progress.
	PrgmInfo_t m_summary;                        // summary of run instance

	// Parsed Cli Data.
	std::vector<std::string> m_files;            // input source files
	bool                     m_doOpt;            // perform optimizations
} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
