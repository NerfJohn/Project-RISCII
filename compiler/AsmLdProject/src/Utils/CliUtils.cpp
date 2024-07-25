/*
 * CliUtils.cpp
 *
 * "Helper functions for parsing CLI arguments into the data model"
 */

#include "Utils/CliUtils.h"

using namespace std;

//==============================================================================

// Definitions to distinguish flags.
#define FLAG_PREFIX    ('-')
#define FLAG_HELP      ("-h")
#define FLAG_LOG_LEVEL ("-ll")

// Definitions to distinguish "-ll" options.
#define LL_ERROR   ("error")
#define LL_WARNING ("warn")
#define LL_INFO    ("info")
#define LL_DEBUG   ("debug")

//==============================================================================
// Identifies type of CLI token string falls into. Defaults to CLI_FILE.
CliType_e CliUtils_asCliType(std::string token) {
	// CLI type to return.
	CliType_e retType = CLI_FILE; // "default" assumption for strings

	// Check token against known flags.
	if (token.compare(FLAG_HELP) == 0) {
		retType = CLI_LONE_FLAG;
	}
	else if (token.compare(FLAG_LOG_LEVEL) == 0) {
		retType = CLI_ARG_FLAG;
	}

	// No flag matches? Check if an "attempted" flag.
	if ((retType == CLI_FILE) &&
		(token.size() > 0) &&
		(token[0] == FLAG_PREFIX)) {
		// Flag syntax, but no matching flags.
		retType = CLI_BAD_FLAG;
	}

	// Return the determined type.
	return retType;
}

//==============================================================================
// Saves a lone flag to model (expressed as string).
RetErr_e CliUtils_saveToModel(DataModel_t& model, std::string flagStr) {
	// Error status to return.
	RetErr_e retCode = RET_GOOD; // innocent till guilty

	// Save result based on given flag.
	if (flagStr.compare(FLAG_HELP) == 0) {model.m_helpFlag = true;}
	else {
		// No matching flag? Indicate the poor input flag.
		retCode = RET_FAIL;
	}

	// Return result of process.
	return retCode;
}

//==============================================================================
// Saves a flag and its argument to model (expressed as strings).
RetErr_e CliUtils_saveToModel(DataModel_t& model,
		                      std::string flagStr,
							  std::string argStr
							 ) {
	// Error status to return.
	RetErr_e retCode = RET_GOOD; // innocent till guilty

	// Save result based on given flag.
	if (flagStr.compare(FLAG_LOG_LEVEL) == 0) {
		// Log Level- set based on recognized options.
		if (argStr.compare(LL_ERROR) == 0) {model.m_logLevel = LOG_ERR;}
		else if (argStr.compare(LL_WARNING) == 0) {model.m_logLevel = LOG_WARN;}
		else if (argStr.compare(LL_INFO) == 0) {model.m_logLevel = LOG_INFO;}
		else if (argStr.compare(LL_DEBUG) == 0) {model.m_logLevel = LOG_DEBUG;}
		else {
			// Unrecognized argument- indicate no match.
			retCode = RET_FAIL;
		}
	}
	else {
		// No matching flag? Indicate the poor input flag.
		retCode = RET_FAIL;
	}

	// Return result of process.
	return retCode;
}
