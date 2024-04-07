/*
 * AnalysisData_t.h
 *
 *  Created on: Apr 4, 2024
 *      Author: John
 */

#ifndef SRC_ANALYSIS_ANALYSISDATA_T_H_
#define SRC_ANALYSIS_ANALYSISDATA_T_H_

#include <stdint.h>
#include <vector>
#include "LabelTable.h"

// Forward declare to break cyclic dependency TODO- better spot? better arch?
class ControlItem;

// TODO- Pertinent data for semantic analysis and checking.
typedef struct {
	// Long-term data (namely for type checking).
	LabelTable m_table;                   // label info for program
	ControlItem* m_glblLabel;              // .glbl control item
	ControlItem* m_heapLabel;              // .heap control item
	int m_textLen;                        // .text section size (in bytes)
	int m_dataLen;                        // .data section size (in bytes)
	int m_bssLen;                         // .bss section size (in bytes)

	// Short-term data (namely for completing analysis).
	ABuildItem* m_lastAddrItem;           // last label-able item seen
} AnalysisData_t;

#endif /* SRC_ANALYSIS_ANALYSISDATA_T_H_ */
