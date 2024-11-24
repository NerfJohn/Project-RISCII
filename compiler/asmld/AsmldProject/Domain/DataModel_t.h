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
 * Data passed in-between states/pieces of the entire program.
 *
 * "Model" of the program with respect to data- any info shared between larger
 * parts of the program is saved and shared via this struct. It is intended for
 * the program to only ever have one, shared instance of this type.
 */
typedef struct {
	// General Summary/Progress.
	uint32_t                 m_numWarns;         // number of found warnings
	uint32_t                 m_numErrs;          // number of found errors
	RetCode_e                m_retCode;          // code to return on exit

	// Parsed Cli Data.
	std::vector<std::string> m_files;            // input source files

} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
