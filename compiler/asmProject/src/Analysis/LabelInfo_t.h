/*
 * LabelInfo_t.h
 *
 *  Created on: Apr 4, 2024
 *      Author: John
 */

#ifndef SRC_ANALYSIS_LABELINFO_T_H_
#define SRC_ANALYSIS_LABELINFO_T_H_

// Forward declare (due to item's referencing data model- TODO better forward spot)?
class ABuildItem;

// TODO- struct holding pertinent data tied to a label.
typedef struct {
	int m_numUses;      // Number of uses of label (vs decls)
	ABuildItem* m_item; // Item tied to label (null if not declared)
} LabelInfo_t;

#endif /* SRC_ANALYSIS_LABELINFO_T_H_ */
