/*
 * CliUtils.h
 *
 * "Helper functions for parsing CLI arguments into the data model"
 */

#ifndef SRC_UTILS_CLIUTILS_H_
#define SRC_UTILS_CLIUTILS_H_

#include "DomainLayer/CliType_e.h"
#include "DomainLayer/DataModel_t.h"
#include "DomainLayer/RetErr_e.h"

/*
 * Identifies type of CLI token string falls into. Defaults to CLI_FILE.
 *
 * Intended primarily to detect different scenarios with command line flags.
 * Only reports a flag type if it matches the flag syntax and/or a known flag.
 * All other strings are assumed filenames.
 *
 * @param token string to identify
 * @return CliType the string falls under
 */
CliType_e CliUtils_asCliType(std::string token);

/*
 * Saves a lone flag to model (expressed as string).
 *
 * Converts string into valid no-argument (ie lone) flag and asserts that flag
 * in the data model. If string matches no known flag, no action is taken and
 * the return code indicates an error.
 *
 * @param model data model of the program
 * @param flagStr lone flag expressed as a string type
 * @return zero on success, non-zero otherwise
 */
RetErr_e CliUtils_saveToModel(DataModel_t& model, std::string flagStr);

/*
 * Saves a flag and its argument to model (expressed as strings).
 *
 * Converts stringified flag and argument as appropriate to include data into
 * the model. If flag string matches no known flag, no action is taken and the
 * return code indicates the error. Same is true with bad arguments.
 *
 * @param model data model of the program
 * @param flagStr flag expressed as a string type
 * @param argStr argument expressed as a string type
 * @return zero on success, non-zero otherwise
 */
RetErr_e CliUtils_saveToModel(DataModel_t& model,
		                      std::string flagStr,
							  std::string argStr
							 );

#endif /* SRC_UTILS_CLIUTILS_H_ */
