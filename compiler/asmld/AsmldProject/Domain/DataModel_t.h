/*
 * DataModel_t.h: Critical data passed between states/steps of the program.
 */

#ifndef DOMAIN_DATAMODEL_T_H_
#define DOMAIN_DATAMODEL_T_H_

#include <string>
#include <vector>

/*
 * Data passed in-between states/pieces of the entire program.
 *
 * "Model" of the program with respect to data- any info shared between larger
 * parts of the program is saved and shared via this struct. It is intended for
 * the program to only ever have one, shared instance of this type.
 */
typedef struct {
	// Parsed Cli Data.
	std::vector<std::string> m_files;            // input source files

} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
