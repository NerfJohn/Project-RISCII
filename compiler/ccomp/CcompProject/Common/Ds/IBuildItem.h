/*
 * IBuildItem.h: Interface defining items used to build ASTs.
 */

#ifndef COMMON_DS_IBUILDITEM_H_
#define COMMON_DS_IBUILDITEM_H_

#include <cstdint>
#include <string>

/*
 * @brief "Interface" (Abstract) class for items used to parse/build ASTs.
 *
 * Interface defines common parent for all items. Also defines expected data for
 * each item. Intended for use mainly with Common/Device/Parser and user defined
 * node base.
 */
class IBuildItem {
public:
	// Data common to all build items.
	int         m_type; // specific build piece
	std::string m_file; // file of origin
	uint32_t    m_line; // line (number) of origin
	
	/*
	 * @brief Destructor- standard issue.
	 */
	virtual ~IBuildItem(void) {/* no actions */}

protected:
	// Interface- only 'created' by derived classes.
	IBuildItem(void)  {
		// Init (no specific values- just known values).
		m_type = 0;
		m_file = "";
		m_line = 0;
	}
};

#endif /* COMMON_DS_IBUILDITEM_H_ */
