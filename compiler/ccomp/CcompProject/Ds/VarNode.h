/*
 * VarNode.h: Node representing declaration/initialization of a data variable.
 */

#ifndef DS_VARNODE_H_
#define DS_VARNODE_H_

#include <stack>
#include "Common/Ds/LexToken.h"
#include "Common/Util/Ptr.h"
#include "Domain/Sym_t.h"
#include "Domain/Type_t.h"
#include "Ds/IAstNode.h"

/*
 * @brief Representation of data variable- defined or initialized.
 */
class VarNode: public IAstNode {
public:
	/*
	 * @brief Constructor- creates node from action stack items.
	 *
	 * Assumes action stack has been grammar checked/parsed for correctness.
	 * Intended to be used with Common/Device/Parser.
	 *
	 * @param actStack action stack with node's build items
	 */
	VarNode(std::stack<Ptr<IBuildItem>>& actStack);

	/*
	 * @brief Analyzes node- prepping information to be checked by check().
	 *
	 * Primarily creates/links symbols while also translating inherited tokens
	 * into useful, comparable values.
	 *
	 * @param model data model of the entire program
	 */
	void analyze(DataModel_t& model);

	/*
	 * @brief Getter for variable's type.
	 *
	 * @return type of variable
	 */
	Ptr<Type_t>& getType(void);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~VarNode(void);

private:
	// Nodes/tokens pulled from action stack.
	std::deque<Ptr<LexToken>> m_typeTkns; // var type tokens
	Ptr<LexToken>             m_name;     // identifier

	// Processed elements.
	Ptr<Type_t>               m_varType;  // 'kind' of variable
	Ptr<Sym_t>                m_sym;      // declared symbol
};

#endif /* DS_VARNODE_H_ */
