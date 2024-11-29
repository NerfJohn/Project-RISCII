/*
 * AAsmNode.cpp: Abstract (interface) class for pieces of core data structure.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/IsaUtil.h"
#include "Util/ModelUtil.h"

#include "Ds/AAsmNode.h"

using namespace std;

//==============================================================================
// Helper function for common immediate value validation.
void AAsmNode::validateImm(DataModel_t& model,
		                   ItemToken const& immItem,
				           ItemToken const& immOp) {
	// Get operation/context of immediate.
	InstrType_e op = IsaUtil_asInstr(immOp.m_lexTkn);

	// Get common vars.
	string   raw  = immItem.m_rawStr;
	string   file = immItem.m_file;
	uint32_t line = immItem.m_line;

	// Analyze the immediate.
	Imm_t procImm;
	if (IsaUtil_toImm(raw, procImm) == RET_ERR_ERROR) {
		Terminate_assert("Analyzed invalid immediate");
	}
	string dbgStr = string("'") + raw + "' -> " + to_string(procImm.m_val);
	Print::inst().log(LOG_DEBUG, file, line, dbgStr);

	// Validate immediate based on operation (or lack thereof).
	bool isValid = (op == INSTR_INVALID) ?
			IsaUtil_isValidWord(procImm) :
			IsaUtil_isValidImm(op, procImm);
	if (isValid == false) {
		string errStr = string("Invalid immediate '") + raw + "'";
		Print::inst().log(LOG_ERROR, file, line, errStr);
		ModelUtil_recordError(model, RET_BAD_IMM);
	}
}

//==============================================================================
// Helper function for common immediate value extraction.
int32_t AAsmNode::getImmVal(ItemToken const& immItem) {
	// Value extracted.
	Imm_t imm;

	// Extract.
	if (IsaUtil_toImm(immItem.m_rawStr, imm) == RET_ERR_ERROR) {
		Terminate_assert("Extracted bad immediate value");
	}

	// Return just value of immediate.
	return imm.m_val;
}

//==============================================================================
// Helper function for pairing nodes to labels.
void AAsmNode::pairOpenLabels(DataModel_t& model,
		                      ItemToken& fileLoc,
							  AddrSpace_e const space) {
	// Pair with any open labels.
	for (Symbol_t* sym : model.m_openLabels) {
		// (Sanity check.)
		if (sym == nullptr) {Terminate_assert("Paired with null symbol");}

		// Set the address space- token order takes care of exact address.
		sym->m_space = space;

		// (Log pairing.)
		string dbgStr = string("Paired '") + sym->m_name + "'";
		Print::inst().log(LOG_DEBUG, fileLoc.m_file, fileLoc.m_line, dbgStr);
	}

	// All open labels claimed- clear list.
	model.m_openLabels.clear();
}
