/*
 * StrUtil.h: Utilities involving string manipulation.
 */

#ifndef COMMON_UTIL_STRUTIL_H_
#define COMMON_UTIL_STRUTIL_H_

#include <string>
#include "Common/Domain/LogType_e.h"
#include "Common/Domain/RetErr_e.h"

/*
 * @brief Translates string into LogType_e (updating passed instance).
 *
 * Updates passed instance with result of translation. Return used to indicate
 * if translation was successfully performed.
 *
 * (Under "Common" as it likely is common to multiple RISCII compiler pieces.)
 *
 * @param type instance to populate with translated value
 * @param str  string to translate
 * @return     RET_ERR_NONE if successful, RET_ERR_ERROR otherwise
 */
RetErr_e StrUtil_asLogType(LogType_e& type, std::string const& str);

#endif /* COMMON_UTIL_STRUTIL_H_ */
