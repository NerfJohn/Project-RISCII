/*
 * DataModel_t.h: Program level data shared between states/steps.
 */

#ifndef DOMAIN_DATAMODEL_T_H_
#define DOMAIN_DATAMODEL_T_H_

#include <cstdint>
#include "Domain/RetCode_e.h"

// TODO- add additional includes as needed for the data stucture.

// TODO- forward declare as needed to keep model as "independent" as possible.

/*
 * @brief Program's core data structure- contains "global" level information.
 *
 * "Model" of program's data. Intended to be instantiated once and shared
 * between program's states/steps. Fields are organized based on function/usage.
 */
typedef struct {
	// General Summary/Progress (required by Common/Device/Terminate.h).
	uint32_t                 m_numWarns;         // number of found warnings
	uint32_t                 m_numErrs;          // number of found errors
	RetCode_e                m_retCode;          // code to return on exit

	// TODO- add fields as needed, organized by function/usage.

} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
