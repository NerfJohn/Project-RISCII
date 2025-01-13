/*
 * LogType_e.h: Different levels of logging that can be performed.
 */

#ifndef DOMAIN_LOGTYPE_E_H_
#define DOMAIN_LOGTYPE_E_H_

/*
 * Enum of different levels used for output granularity.
 *
 * Levels are intended to establish a "hierarchy" of info importance- allowing
 * unimportant info to be filtered as needed. In this case, lower values
 * indicate higher importance (with the highest filtered ALL info).
 */
typedef enum {
	// Real values.
	LOG_SILENT  = 0, // NO log output
	LOG_ERROR   = 1, // only log errors
	LOG_WARNING = 2, // only log errors + warnings
	LOG_INFO    = 3, // log "metrics" of process
	LOG_DEBUG   = 4  // log all available program output
} LogType_e;


#endif /* DOMAIN_LOGTYPE_E_H_ */
