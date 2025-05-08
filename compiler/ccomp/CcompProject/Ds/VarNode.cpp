/*
 * VarNode.cpp: Node representing declaration/initialization of a data variable.
 */

#include "Domain/ParseState_e.h"
#include "Util/AppUtil.h"
#include "Util/DsUtil.h"
#include "Util/TypeUtil.h"

#include "Ds/VarNode.h"

using namespace std;

//==============================================================================
// Constructor- creates node from action stack items.
VarNode::VarNode(std::stack<Ptr<IBuildItem>>& actStack) {
	// Init members.
	m_typeTkns = {};
	m_name     = Ptr<LexToken>(nullptr);
	m_varType  = Ptr<Type_t>(nullptr);
	m_sym      = Ptr<Sym_t>(nullptr);

	m_type = PARSE_ACT_VDEC;
	m_file = "";
	m_line = 0;

	// Claim relevant nodes/tokens.
	bool doBreak = false;
	while (actStack.size() > 0) {
		// Get next item.
		Ptr<IBuildItem> item = actStack.top();

		// Claim (as applicable).
		switch(item->m_type) {
		case LEX_TKN_SEMICOLON:
			// "Parsing fluff".
			break;
			case LEX_TKN_ID:
				// Func ID.
				m_name = item.toType<LexToken>();
				break;
			case LEX_TKN_STAR:
				// Type.
				m_typeTkns.push_front(item.toType<LexToken>());
				break;
			case LEX_TKN_INT:
			case LEX_TKN_VOID:
				// Type + end of def/decl.
				m_typeTkns.push_front(item.toType<LexToken>());
				m_file  = item->m_file;
				m_line  = item->m_line;
				doBreak = true;
				break;
			default:
				// Unknown type- bug!
				AppUtil_exitBug("VarNode() bad type");
				break;
		}

		// Consume with viewing.
		actStack.pop();

		// Stop when signaled.
		if (doBreak) {break;}
	}

	// Verify critical pieces.
	if (0 == m_typeTkns.size()) {AppUtil_exitBug("VarNode() no type");}
	if (m_name.isNull())        {AppUtil_exitBug("VarNode() no name");}
}

//==============================================================================
// Analyzes node- prepping information to be checked by check().
void VarNode::analyze(DataModel_t& model) {
	// Interpret type.
	m_varType = Ptr<Type_t>(new Type_t());
	if (m_varType.isNull()) {AppUtil_exitBug("analyze(Var) null type");}
	TypeUtil_initCore(*m_varType, m_typeTkns); // explicit  l-values
	m_varType->m_hasLoc = true;

	// Create symbol.
	m_sym = Ptr<Sym_t>(new Sym_t());
	if (m_sym.isNull()) {AppUtil_exitBug("analyze(Var) null sym");}
	m_sym->m_type      = m_varType; // node always defines variable
	m_sym->m_isDefined = true;
	m_sym->m_file      = m_file;
	m_sym->m_line      = m_line;

	// Add Symbol.
	DsUtil_addSym(model, m_name->m_rawData, m_sym);
}

//==============================================================================
// Getter for variable's type.
Ptr<Type_t>& VarNode::getType(void) {
	// Return variable's type.
	return m_varType;
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
VarNode::~VarNode(void) {/* no actions */}
