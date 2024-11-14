/*
 * DataModel_t.h: Critical data passed between states/steps of the program.
 */

#ifndef DOMAIN_DATAMODEL_T_H_
#define DOMAIN_DATAMODEL_T_H_

#include <string>
#include <vector>

// TODO
typedef struct {
	// Parsed Cli Data.
	std::vector<std::string> m_files;            // input source files

} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
