/*
 * TextNode.h: Node representing raw text to be copied into result file.
 */

#ifndef DS_TEXTNODE_H_
#define DS_TEXTNODE_H_

#include <stack>
#include "Ds/AAstNode.h"
#include "Ds/ItemToken.h"

// TODO
class TextNode: public AAstNode {
public:
	// Enforce node class as "no copy".
	TextNode(TextNode const&)       = delete;
	void operator=(TextNode const&) = delete;

	// TODO
	TextNode(std::stack<ABuildItem*>& itemStack);

	// TODO
	BuildType_e getType(void);

	/*
	 * General destructor- public to allow for generic node deletion.
	 */
	~TextNode(void);

private:
	// Raw items REQUIRED to compose raw text.
	std::deque<ItemToken*>   m_reqTxt; // undo "stack inversion" of text
};

#endif /* DS_TEXTNODE_H_ */
