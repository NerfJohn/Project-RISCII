/*
 * DataWordType_e.h
 *
 * "Defines possible ranges for a 16-bit integer"
 */

#ifndef SRC_DOMAINLAYER_DATAWORDTYPE_E_H_
#define SRC_DOMAINLAYER_DATAWORDTYPE_E_H_


/*
 * Defines ranges for a 16-bit integer. Used to define integer range for checks.
 *
 * Intended to be used to define the valid range of a 16-bit number (assuming
 * number is being stored/analyzed with more than 16-bits). Contextually, it
 * may also be used to define how a 16-bit number should be interpreted.
 */
typedef enum {
	// Core Values.
	DATA_WORD_UNSIGNED, // integer must fit within 16-bit unsigned range
	DATA_WORD_ANY,      // integer just needs to be expressible with 16 bits
} DataWordType_e;


#endif /* SRC_DOMAINLAYER_DATAWORDTYPE_E_H_ */
