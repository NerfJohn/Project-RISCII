/*
 * LabelData_t.h
 *
 * "Info related to an assembly label"
 */

#ifndef SRC_DOMAINLAYER_LABELDATA_T_H_
#define SRC_DOMAINLAYER_LABELDATA_T_H_

#include <string>

// Forward declare- break cyclic includes (first "ptr needed" spot in chain).
class IBuildItem;

/*
 * Info associated to a specified label name.
 *
 * Info is intended to be populated and referenced during analysis/checking and
 * translation. It is intended to be used in conjunction with a LabelTable
 * object.
 */
typedef struct {
	// General info for local/global checks.
	bool        m_isDefined;
	bool        m_isUsed;

	// Info regarding location of definition.
	std::string m_origFileLoc;
	uint32_t    m_origLineLoc;

	// Item being addressed by label.
	IBuildItem* m_labelItem;
} LabelData_t;

#endif /* SRC_DOMAINLAYER_LABELDATA_T_H_ */
