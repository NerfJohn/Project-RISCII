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
// Analyzes node- validating local arguments/symbols.
void AAsmNode::localAnalyze(DataModel_t& model, SymTable& table) { /* --- */}

//==============================================================================
// Handles local links/symbols- modifying and linking to local symbols.
void AAsmNode::localLink(DataModel_t& model, SymTable& table) { /* --- */}

//==============================================================================
// Handles global links/symbols- finishing overall symbol linkage.
void AAsmNode::globalLink(DataModel_t& model) { /* --- */}

//==============================================================================
// Finishing program checks- requesting deletions as needed to slim program.
CleanAction_e AAsmNode::globalClean(DataModel_t& model) {return CLEAN_KEEP;}

//==============================================================================
// Analyze program- generating addresses for each symbol.
void AAsmNode::imageAddress(DataModel_t& model) { /* --- */}

//==============================================================================
// Assembles program- generating binary values in the data model.
void AAsmNode::imageAssemble(DataModel_t& model) { /* --- */}

//==============================================================================
// General destructor- public to allow for generic node deletion.
AAsmNode::~AAsmNode(void) { /* --- */}

//==============================================================================
// Common helper function to get/validate register value.
uint8_t AAsmNode::getReg(DataModel_t& model, ItemToken const& reg) {
	// Result of the process.
	uint8_t retReg = 0;

	// (Item is register, right?)
	if (reg.m_lexTkn != TOKEN_REGISTER) {Terminate_assert("getReg() w/o reg");}

	// Attempt to get register value.
	RetErr_e retErr = IsaUtil_toReg(reg.m_rawStr, retReg);

	// Log error if failed.
	if (retErr) {
		string errStr = string("Invalid register '") + reg.m_rawStr + "'";
		Print::inst().log(LOG_ERROR, reg.m_file, reg.m_line, errStr);
		ModelUtil_recordError(model, RET_BAD_REG);
	}

	// Return resulting register value.
	return retReg;
}

//==============================================================================
// Common helper function to get/validate immediate values.
int32_t AAsmNode::getImm(DataModel_t& model,
		                 ItemToken const& imm,
			             ItemToken const& op) {
	// (Item is immediate, right?)
	if (imm.m_lexTkn != TOKEN_IMMEDIATE) {Terminate_assert("getImm() w/o imm");}

	// Get operation/context of immediate.
	InstrType_e opType = IsaUtil_asInstr(op.m_lexTkn);

	// Get the immediate value (should be shoe-in due to lexing).
	Imm_t fullImm;
	if (IsaUtil_toImm(imm.m_rawStr, fullImm) == RET_ERR_ERROR) {
		Terminate_assert("toImm() on lexed imm");
	}

	// Validate immediate value (within context).
	bool isValid = (opType == INSTR_INVALID)           ?
			        IsaUtil_isValidWord(fullImm)       :
			        IsaUtil_isValidImm(opType, fullImm);

	// Log error if validation failed.
	if (isValid == false) {
		string errStr = string("Invalid immediate '") + imm.m_rawStr + "'";
		Print::inst().log(LOG_ERROR, imm.m_file, imm.m_line, errStr);
		ModelUtil_recordError(model, RET_BAD_IMM);
	}

	// Return resulting immediate value.
	return fullImm.m_val;
}

//==============================================================================
// Common helper function to claim unpaired labels.
void AAsmNode::pairLabels(DataModel_t& model,
		                  ItemToken const& loc,
				          AddrSpace_e const space) {
	// Pair with ALL unpaired labels.
	for (Symbol_t* sym : model.m_openLabels) {
		// Set label's/symbol's address space to caller's.
		IF_NULL(sym, "pairLabels() with null symbol");
		sym->m_space = space;

		// Pairing is a "reference" of sorts- adjust counter.
		sym->m_numRefs++;

		// (Log pairing.)
		string dbgStr = string("Paired '") + sym->m_name + "'";
		Print::inst().log(LOG_DEBUG, loc.m_file, loc.m_line, dbgStr);
	}

	// Paired all labels- clear list.
	model.m_openLabels.clear();
}

//==============================================================================
// Common helper function to free symbols/ptrs to symbols.
void AAsmNode::freeSymbol(Symbol_t*& sym) {
	// Adjust reference counter.
	IF_NULL(sym, "free() on null symbol");
	sym->m_numRefs--;

	// Nullify ptr- freeing actual symbol if last reference.
	if (sym->m_numRefs == 0) {delete sym;}
	sym = nullptr;
}
