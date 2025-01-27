/*
 * AAstNode.h: Abstract class defining building blocks of AST.
 */

#ifndef DS_AASTNODE_H_
#define DS_AASTNODE_H_

#include "Ds/ABuildItem.h"

// TODO
class AAstNode: public ABuildItem {
public:
	// TODO
	virtual BuildType_e getType(void) = 0;

	/*
	 * General destructor- public to allow for generic node deletion.
	 */
	virtual ~AAstNode(void);
};

#endif /* DS_AASTNODE_H_ */
