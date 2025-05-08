/*
 * FuncNode.cpp: Node representing a function (declaration or definition).
 */

#include "Domain/ParseState_e.h"
#include "Util/AppUtil.h"
#include "Util/DsUtil.h"
#include "Util/TypeUtil.h"

#include "Ds/FuncNode.h"

using namespace std;

//==============================================================================

// Constants used in decision logic.
#define FIRST_SYM_REF_CNT 3 // funcNode + symTable + model while in function

//==============================================================================
// Constructor- creates node from action stack items.
FuncNode::FuncNode(std::stack<Ptr<IBuildItem>>& actStack) {
	// Init members.
	m_typeTkns = {};
	m_name     = Ptr<LexToken>(nullptr);
	m_params   = {};
	m_isDef    = false;

	m_type = PARSE_ACT_FDEC;
	m_file = "";
	m_line = 0;

	// Claim relevant nodes/tokens.
	bool doBreak = false;
	while (actStack.size() > 0) {
		// Get next item.
		Ptr<IBuildItem> item = actStack.top();

		// Claim (as applicable).
		switch(item->m_type) {
			case LEX_TKN_RCURLY:
			case LEX_TKN_LPAREN:
			case LEX_TKN_RPAREN:
			case LEX_TKN_SEMICOLON:
			case LEX_TKN_COMMA:
				// "Parsing fluff".
				break;
			case LEX_TKN_LCURLY:
				// Is definition.
				m_isDef = true;
				break;
			case PARSE_ACT_VDEC:
				// Save params.
				m_params.push_front(item.toType<VarNode>());
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
				AppUtil_exitBug("FuncNode() bad type");
				break;
		}

		// Consume with viewing.
		actStack.pop();

		// Stop when signaled.
		if (doBreak) {break;}
	}

	// Verify critical pieces.
	if (0 == m_typeTkns.size()) {AppUtil_exitBug("FuncNode() no type");}
	if (m_name.isNull())        {AppUtil_exitBug("FuncNode() no name");}
}

//==============================================================================
// Analyzes node- prepping information to be checked by check().
void FuncNode::analyze(DataModel_t& model) {
	// Interpret function type.
	m_varType = Ptr<Type_t>(new Type_t());
	if (m_varType.isNull()) {AppUtil_exitBug("analyze(Func) null func type");}
	m_varType->m_type   = CORE_TYPE_VOID; // funcs = void* r-values
	m_varType->m_ptrCnt = 1;
	m_varType->m_hasLoc = false;

	// Interpret return type.
	m_retType = Ptr<Type_t>(new Type_t());
	if (m_retType.isNull()) {AppUtil_exitBug("analyze(Func) null ret type");}
	TypeUtil_initCore(*m_retType, m_typeTkns); // explicit- but always r-value
	m_retType->m_hasLoc = false;

	// Add Symbol.
	m_sym = Ptr<Sym_t>(new Sym_t());
	if (m_sym.isNull()) {AppUtil_exitBug("analyze(Func) null sym");}
	m_sym->m_type      = m_varType; // node declares or defines function
	m_sym->m_isDefined = m_isDef;
	m_sym->m_file      = m_file;
	m_sym->m_line      = m_line;
	DsUtil_addSym(model, m_name->m_rawData, m_sym);

	// Analyze Function Internals.
	if (APP_OK) {
		// Enter.
		model.m_fSym = m_sym;
		model.m_syms.scopePush();

		// Analyze Params.
		for (Ptr<VarNode> param : m_params) {param->analyze(model);}

		// Created symbol? populate signature (AFTER analyzing ret/params).
		if (m_sym.refCnt() == FIRST_SYM_REF_CNT) {
			m_sym->m_retType = m_retType;
			for (Ptr<VarNode> param : m_params) {
				m_sym->m_params.push_back(param->getType());
			}
		}

		// Exit.
		model.m_syms.scopePop();
		model.m_fSym = Ptr<Sym_t>(nullptr);
	}
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
FuncNode::~FuncNode(void) {/* no actions */}
