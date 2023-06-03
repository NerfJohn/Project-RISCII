/*
 * IBuildItem.h
 */

#ifndef IBUILDITEM_H_
#define IBUILDITEM_H_

#include <stdint.h>
#include <string>

/*
 * Interface class for declaring functions to build the Abstract Syntax Tree.
 *
 * Interface ensures implementing/derived classes can be used to build the
 * Abstract Syntax Tree (AST). Common functions are required to get/share
 * necessary information to build the AST smoothly and successfully.
 */
class IBuildItem {
public:
	/*
	 * Getters to acquire needed information for building the AST.
	 *
	 * Required information consists of identifying data (ie type), the original
	 * line number location of the information, and extra data associated with
	 * the item (eg value of a literal). This information is passed up/shared
	 * from the original tokens to the created nodes.
	 *
	 * Note that "build type" should refer to a token or parse action type (as
	 * defined by enums ScanTableStates and ParseActions).
	 *
	 * @return requested raw (ie non-ptr) information
	 */
	virtual std::uint8_t getBuildType(void) = 0;
	virtual int getLineNum(void) = 0;
	virtual std::string getValue(void) = 0;

	/*
	 * Std. Destructor.
	 */
	virtual ~IBuildItem() {/* Empty dtor */}
};

#endif /* IBUILDITEM_H_ */
