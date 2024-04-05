/*
 * AnalysisData_t.h
 *
 *  Created on: Apr 4, 2024
 *      Author: John
 */

#ifndef SRC_ANALYSIS_ANALYSISDATA_T_H_
#define SRC_ANALYSIS_ANALYSISDATA_T_H_

#include <vector>
#include "LabelTable.h"

// TODO- Pertinent data for semantic analysis and checking.
typedef struct {
	// Long-term data (namely for type checking).
	LabelTable m_table;                   // label info for program
	std::string m_glblLabel;              // .glbl label name
	std::string m_heapLabel;              // .heap label name
	int m_textLen;                        // .text section size (in bytes)
	int m_dataLen;                        // .data section size (in bytes)
	int m_bssLen;                         // .bss section size (in bytes)

	// Short-term data (namely for completing analysis).
	ABuildItem* m_lastAddrItem;           // last label-able item seen
} AnalysisData_t;

#endif /* SRC_ANALYSIS_ANALYSISDATA_T_H_ */
