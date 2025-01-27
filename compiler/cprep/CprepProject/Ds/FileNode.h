/*
 * FileNode.h: Node representing one file/translation unit.
 */

#ifndef DS_FILENODE_H_
#define DS_FILENODE_H_

#include <stack>
#include "Ds/AAstNode.h"

// TODO
class FileNode: public AAstNode {
public:
	// Enforce node class as "no copy".
	FileNode(FileNode const&)       = delete;
	void operator=(FileNode const&) = delete;

	// TODO
	FileNode(std::stack<ABuildItem*>& itemStack);

	// TODO
	BuildType_e getType(void);

	/*
	 * General destructor- public to allow for generic node deletion.
	 */
	~FileNode(void);

private:
	// Raw items REQUIRED to compose file.
	std::deque<AAstNode*>   m_reqNodes; // undo "stack inversion" of node order
};

#endif /* DS_FILENODE_H_ */
