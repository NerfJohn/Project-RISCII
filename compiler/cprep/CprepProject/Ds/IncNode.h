/*
 * IncNode.h: Node representing include pre-processing directive.
 */

#ifndef DS_INCNODE_H_
#define DS_INCNODE_H_

#include <stack>
#include "Ds/AAstNode.h"
#include "Ds/ItemToken.h"

// TODO
class IncNode: public AAstNode {
public:
	// Enforce node class as "no copy".
	IncNode(IncNode const&)        = delete;
	void operator=(IncNode const&) = delete;

	// TODO
	IncNode(std::stack<ABuildItem*>& itemStack);

	// TODO
	BuildType_e getType(void);

	/*
	 * General destructor- public to allow for generic node deletion.
	 */
	~IncNode(void);

private:
	// Raw items REQUIRED to compose include directive.
	ItemToken* m_reqFname;
};

#endif /* DS_INCNODE_H_ */
