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
#include "DomainLayer/LogType_e.h"

// Forward declaration- keep data model "independent".
class IBuildItem;

/*
 * Data shared between the program's different stages.
 *
 * This struct is intended to be created once and used to link the various
 * layers with regards to referenced data. While aggressive in size and access
 * rights, it provides a quick and clean interface for data to flow.
 */
typedef struct {
	// Values related to warning and error detection/reporting.
	uint32_t                 m_numWarnings;     // # of warnings found
	uint32_t                 m_numErrors;       // # of errors found
	ExitReasonType_e         m_firstReason;     // reason for build failure

	// Values related to parsed command line input.
	bool                     m_helpFlag;        // help flag presence
	bool                     m_weFlag;          // warn as errors flag presence
	LogType_e                m_logLevel;        // granularity of printing
	std::vector<std::string> m_inFiles;         // passed in input files
	std::string              m_outFile;         // filename to use for binary

	// Values related to parsing/analyzing/checking the entire program.
	std::vector<IBuildItem*> m_items;           // item-ized program
	uint32_t                 m_numTextBytes;    // bytes needed for text section

	// Values related to prepping for file writing.
	std::vector<uint16_t>    m_textSection;     // translated text section
} DataModel_t;


#endif /* SRC_DOMAINLAYER_DATAMODEL_T_H_ */
