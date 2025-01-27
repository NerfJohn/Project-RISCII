/*
 * ABuildItem.h: Abstract class defining build elements of nodes.
 */

#ifndef DS_ABUILDITEM_H_
#define DS_ABUILDITEM_H_

#include <cstdint>
#include <string>
#include "Domain/BuildType_e.h"

// TODO
class ABuildItem {
public:
	// File/line origin of the item.
	std::string m_file;
	uint32_t    m_line;

	// TODO
	virtual BuildType_e getType(void) = 0;

	// TODO
	virtual ~ABuildItem(void);
};

#endif /* DS_ABUILDITEM_H_ */
