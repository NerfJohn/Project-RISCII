/*
 * StrUtil.cpp: Utilities involving string manipulation.
 */

#ifndef COMMON_UTIL_STRUTIL_CPP_
#define COMMON_UTIL_STRUTIL_CPP_

#include "Common/Util/StrUtil.h"

using namespace std;

//==============================================================================

// Definitions for "string to LogType_e" conversions.
#define LEVEL_SILENT   ("silent")
#define LEVEL_ERROR    ("error")
#define LEVEL_WARNING  ("warning")
#define LEVEL_INFO     ("info")
#define LEVEL_DEBUG    ("debug")

//==============================================================================
// Translates string into LogType_e (updating passed instance).
RetErr_e StrUtil_asLogType(LogType_e& type, std::string const& str) {
	// Return value.
	RetErr_e ret = RET_ERR_NONE;

	// Attempt to convert the string.
	if      (str.compare(LEVEL_SILENT)  == 0) {type = LOG_SILENT;}
	else if (str.compare(LEVEL_ERROR)   == 0) {type = LOG_ERROR;}
	else if (str.compare(LEVEL_WARNING) == 0) {type = LOG_WARNING;}
	else if (str.compare(LEVEL_INFO)    == 0) {type = LOG_INFO;}
	else if (str.compare(LEVEL_DEBUG)   == 0) {type = LOG_DEBUG;}
	else    {ret = RET_ERR_ERROR;}

	// Return result.
	return ret;
}

#endif /* COMMON_UTIL_STRUTIL_CPP_ */
