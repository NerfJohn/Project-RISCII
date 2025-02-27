/*
 * IBuildItem.h: Interface defining Common/Device/Parser.h build stack items.
 */

#ifndef COMMON_DS_IBUILDITEM_H_
#define COMMON_DS_IBUILDITEM_H_

#include <cstdint>
#include <string>

/*
 * @brief Interface class for items used to parse/build Abstract Syntax Trees.
 *
 * Base item pushed/popped from action stack (managed by Common/Device/Parser).
 * Intended to be a base class to Common/Ds/LexToken.h and user defined node
 * types.
 */
class IBuildItem {
public:
	// Specific item type- 'enum' for ease of node/tree-building.
	int         m_type;
	
	// Origin of item- presumed to be a file.
	std::string m_file;
	uint32_t    m_line;
	
	/*
	 * @brief Std destructor
	 */
	virtual ~IBuildItem(void) {/* no actions */}

protected:
	// Interface- only 'created' by derived classes.
	IBuildItem(void)  {/* no actions */}
};

#endif /* COMMON_DS_IBUILDITEM_H_ */
