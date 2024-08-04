/*
 * InitType_e.h
 *
 * "Defines different types of data (based on initialization)"
 */

#ifndef SRC_DOMAINLAYER_INITTYPE_E_H_
#define SRC_DOMAINLAYER_INITTYPE_E_H_

/*
 * Definitions for initialization
 */
typedef enum {
	// Core values
	INIT_TYPE_DATA, // data is initialized as part of the data section
	INIT_TYPE_BSS,  // data is "initialized" as part of the bss section
} InitType_e;

#endif /* SRC_DOMAINLAYER_INITTYPE_E_H_ */
