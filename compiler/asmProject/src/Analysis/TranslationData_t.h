/*
 * TranslationData_t.h
 *
 *  Created on: Apr 7, 2024
 *      Author: John
 */

#ifndef SRC_ANALYSIS_TRANSLATIONDATA_T_H_
#define SRC_ANALYSIS_TRANSLATIONDATA_T_H_

#include <stdint.h>

// TODO- Pertinent data to track while translating/creating a binary.
typedef struct {
	// Address data for assigning ROM/RAM addresses (for label resolution).
	uint16_t m_nextTextAddr;
	uint16_t m_nextDataAddr;
	uint16_t m_nextBssAddr;
} TranslationData_t;

#endif /* SRC_ANALYSIS_TRANSLATIONDATA_T_H_ */
