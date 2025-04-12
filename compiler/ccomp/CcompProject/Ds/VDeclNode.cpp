/*
 * VDeclNode.cpp: Node representing variable declaration.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/InfoUtil.h"
#include "Domain/ParseState_e.h"

#include "Ds/VDeclNode.h"

using namespace std;

//==============================================================================
// Constructor- creates node from action stack items.
VDeclNode::VDeclNode(std::stack<IBuildItem*>& actStack, DataModel_t& model) {
	// Init members.
	m_cType.m_type       = PRIMITIVE_CHAR;
	m_cType.m_isUnsigned = false;
	m_cType.m_isVolatile = false;
	m_cType.m_ptrCnt     = 0;
	m_cType.m_isArray    = false;
	m_id                 = nullptr;
	m_arrSize            = nullptr;
	m_initVal            = nullptr;
	m_type               = PARSE_ACT_VDEC;
	m_file               = "";
	m_line               = 0;

	// Claim as many items as possible (ie broken from within).
	bool doBreak  = false;
	bool varFound = false;
	while (actStack.size() > 0) {
		// Get next item.
		IBuildItem* item = actStack.top();
		IF_NULL(item, "VDeclNode() null item");

		// Claim item if supposed to be part of node.
		switch (item->m_type) {
			case PARSE_ACT_LIT:
				// Init value.
				m_initVal = (IAstNode*)(item);
				break;
			case LEX_TKN_INT_LIT:
				// Array size specifier.
				m_cType.m_isArray = true;
				m_arrSize         = (LexToken*)(item);
				break;
			case LEX_TKN_ID:
				// Variable in question.
				m_id     = (LexToken*)(item);
				varFound = true;
				break;
			case LEX_TKN_CHAR:
				// 'char' var.
				m_cType.m_type = PRIMITIVE_CHAR;
				delete           item;
				break;
			case LEX_TKN_INT:
				// 'int' var.
				m_cType.m_type = PRIMITIVE_INT;
				delete           item;
				break;
			case LEX_TKN_VOID:
				// 'void' var.
				m_cType.m_type = PRIMITIVE_VOID;
				delete           item;
				break;
			case LEX_TKN_STAR:
				// Extra "ptr level".
				m_cType.m_ptrCnt++;
				delete item;
				break;
			case LEX_TKN_UNSIGNED:
				// Modify core variable.
				if (m_cType.m_isUnsigned) {
					Print::inst().log(LOG_WARNING,
							          item->m_file,
									  item->m_line,
									  "Redundant 'unsigned'");
					InfoUtil_recordWarn(model.m_summary);
				}
				m_cType.m_isUnsigned = true;
				delete                 item;
				break;
			case LEX_TKN_VOLATILE:
				// Modify core variable.
				if (m_cType.m_isVolatile) {
					Print::inst().log(LOG_WARNING,
							          item->m_file,
									  item->m_line,
									  "Redundant 'volatile'");
					InfoUtil_recordWarn(model.m_summary);
				}
				m_cType.m_isVolatile = true;
				delete                 item;
				break;
			case LEX_TKN_SEMICOLON:
				// End of this or end of other node.
				if (varFound) {doBreak = true;}
				else          {delete    item;}
				break;
			case LEX_TKN_ASSIGN:
			case LEX_TKN_LSQUARE:
			case LEX_TKN_RSQUARE:
				// Parsing sugar.
				delete item;
				break;
			default:
				// Unrecognized item- end of literal.
				doBreak = true;
				break;
		}

		// Consume any parsed tokens.
		if (doBreak == false) {actStack.pop();}

		// Break out as directed.
		if (doBreak) {break;}
	}

	// Ensure var name is saved.
	IF_NULL(m_id, "VDeclNode() no var name");
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
VDeclNode::~VDeclNode(void) {
	// Delete held tokens.
	delete m_id;
	if (m_arrSize != nullptr) {delete m_arrSize;}
	if (m_initVal != nullptr) {delete m_initVal;}
}
