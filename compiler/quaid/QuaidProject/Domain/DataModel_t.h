/*
 * DataModel_t.h: Critical data passed between states/steps of the program.
 */

#ifndef DOMAIN_DATAMODEL_T_H_
#define DOMAIN_DATAMODEL_T_H_

#include <string>
#include <vector>
#include "Common/Domain/LogType_e.h"
#include "Common/Domain/PrgmInfo_t.h"
#include "Domain/OptLevel_e.h"
#include "Domain/ProcDepth_e.h"

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
	LogType_e                m_logLvl;           // log level of program
	std::vector<std::string> m_sFiles;           // input (S) source files
	ProcDepth_e              m_depth;            // depth of processing to do
	std::vector<std::string> m_iDefs;            // pre-processor definitions
	std::vector<std::string> m_iDirs;            // include directories
	bool                     m_doDbg;            // enables adding debug symbols
	std::string              m_outFile;          // name of created file
	OptLevel_e               m_optLvl;           // level of optimization
} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
