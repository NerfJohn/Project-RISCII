/*
 * DataModel_t.h
 *
 * "Data "highway" holding and transferring data between program's layers"
 */

#ifndef SRC_DOMAINLAYER_DATAMODEL_T_H_
#define SRC_DOMAINLAYER_DATAMODEL_T_H_

#include <string>
#include <vector>
#include "DomainLayer/ExitReasonType_e.h"

/*
 * Data shared between the program's different stages.
 *
 * This struct is intended to be created once and used to link the various
 * layers with regards to referenced data. While aggressive in size and access
 * rights, it provides a quick and clean interface for data to flow.
 */
typedef struct {
	// Values related to warning and error detection/reporting.
	uint32_t         m_numWarnings;     // # of warnings found
	uint32_t         m_numErrors;       // # of errors found
	ExitReasonType_e m_firstReason;     // reason for build failure

	// Values related to parsed command line input.
	std::vector<std::string> m_inFiles; // passed in input files
} DataModel_t;


#endif /* SRC_DOMAINLAYER_DATAMODEL_T_H_ */
