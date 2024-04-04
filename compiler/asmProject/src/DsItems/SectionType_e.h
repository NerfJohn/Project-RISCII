/*
 * SectionType_e.h
 *
 *  Created on: Apr 3, 2024
 *      Author: John
 */

#ifndef SRC_DSITEMS_SECTIONTYPE_E_H_
#define SRC_DSITEMS_SECTIONTYPE_E_H_

// TODO- Section of the binary (or live run of binary).
typedef enum {
	// Core Values.
	SECTION_TEXT,
	SECTION_RODATA,
	SECTION_DATA,
	SECTION_BSS,

	// Special Values.
	SECTION_INVALID
} SectionType_e;


#endif /* SRC_DSITEMS_SECTIONTYPE_E_H_ */
