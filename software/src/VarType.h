/*
 * VarType.h
 */

#ifndef VARTYPE_H_
#define VARTYPE_H_

#include <string>

/*
 * Enum to represent the possible variable types of the compiler.
 *
 * Enum can be used to clearly label the type represented in a class. Static
 * class included in the file provides additional functions to make the enum
 * more versatile (ie making the types print-able).
 */
enum VarType_e {
	TYPE_INT,
	TYPE_CHAR,
	TYPE_UINT,
	TYPE_UCHAR,
	TYPE_LITERAL
};

/*
 * Static class to provide an easy way to interact/use the VarType_e enum.
 *
 * Class provides functions to make the VarType_e enum more versatile. Namely,
 * the class provides a way to string-ify the enum for use in logging messages.
 */
class VarType {
public:
	/*
	 * Static function to string-ify a var type enum.
	 *
	 * Function simply creates a string version of the associated enum. This
	 * provides an easy way to add enum info to message logs. If the passed in
	 * value doesn't match an enum value, the function asserts.
	 *
	 * @param type var type enum to be string-ified
	 * @return string version of the passed in enum
	 */
	static std::string toString(VarType_e type);

	/*
	 * Std. Destructor.
	 */
	virtual ~VarType(void) {/* Empty dtor */}

private:
	// Private constructor to prevent instantiation.
	VarType(void) {/* Empty ctor */}
};



#endif /* VARTYPE_H_ */
