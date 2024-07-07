/*
 * LogType_e.h
 *
 * "Various levels of logging available to the program"
 */

#ifndef SRC_DOMAINLAYER_LOGTYPE_E_H_
#define SRC_DOMAINLAYER_LOGTYPE_E_H_

/*
 * Defines different levels of logging available per logged information.
 */
typedef enum {
	// Core values.
	LOG_ERR  = 0, // Errors
	LOG_WARN = 1, // Warnings
} LogType_e;

#endif /* SRC_DOMAINLAYER_LOGTYPE_E_H_ */
