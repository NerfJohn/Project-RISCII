/*
 * LogType_e.h: Type specifying various levels of log output.
 */

#ifndef COMMON_DOMAIN_LOGTYPE_E_H_
#define COMMON_DOMAIN_LOGTYPE_E_H_

/*
 * @brief Enum of levels of log output- organized as a hirearchy.
 *
 * Levels act as hierarchy- with more verbose log types having larger values.
 * This allows log types to be compared to "encapsulate" less verbose types
 * within more verbose types.
 */
typedef enum {
	// Real values.
	LOG_SILENT  = 0, // NO log output
	LOG_ERROR   = 1, // only log errors
	LOG_WARNING = 2, // only log errors + warnings
	LOG_INFO    = 3, // log "metrics" of process
	LOG_DEBUG   = 4  // log all available program output
} LogType_e;

#endif /* COMMON_DOMAIN_LOGTYPE_E_H_ */
