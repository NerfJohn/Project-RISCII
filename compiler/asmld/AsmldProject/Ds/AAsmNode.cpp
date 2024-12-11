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
// Writes debug table information to (assumed open) binary image.
void AAsmNode::optPrintDebug(void) { /* --- */}

//==============================================================================
// Determines if node (+related section nodes) should be removed.
CleanAction_e AAsmNode::optRemoveLabel(DataModel_t& model) {return CLEAN_KEEP;}

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

	// Get the immediate value (should be shoe-in due to lexing).
	Imm_t fullImm;
	if (IsaUtil_toImm(imm.m_rawStr, fullImm) == RET_ERR_ERROR) {
		Terminate_assert("toImm() on lexed imm");
	}

	// Validate immediate value (within context).
	InstrType_e opType  = IsaUtil_asInstr(op.m_lexTkn);
	bool        isValid = false;
	switch (op.m_lexTkn) {
		case TOKEN_KW_DATA:  isValid = IsaUtil_isValidWord(fullImm); break;
		case TOKEN_KW_BSS:   isValid = (fullImm.m_val >= 0);         break;
		default:
			// Instruction based immediate- requires opcode knowledge.
			isValid = IsaUtil_isValidImm(opType, fullImm); break;
	}

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
// Common helper function to link a local label, finding its symbol.
void AAsmNode::linkLocal(SymTable& table,
		                 ItemToken const& label,
						 Symbol_t*& sym) {
	// (Item is label, right?)
	if (label.m_lexTkn != TOKEN_LABEL) {Terminate_assert("llink() w/o label");}

	// Search table for symbol to link to (may not find if global ref).
	if (table.getSym(label.m_rawStr, sym) == RET_ERR_NONE) {
		// Symbol now referenced- update counter.
		sym->m_numRefs++;

		// Log link.
		string dbgStr = string("Local link '") +
				        label.m_rawStr         +
						"' (to "               +
						sym->m_file            +
						"/"                    +
						to_string(sym->m_line) +
						")";
		Print::inst().log(LOG_DEBUG, label.m_file, label.m_line, dbgStr);
	}
}

//==============================================================================
// Common helper function to link a global label, finding its symbol.
void AAsmNode::linkGlobal(DataModel_t& model,
		                  ItemToken const& label,
						  Symbol_t*& sym) {
	// (Item is label, right?)
	if (label.m_lexTkn != TOKEN_LABEL) {Terminate_assert("glink() w/o label");}

	// Attempt to re-link weak references.
	if ((sym != nullptr) && (sym->m_isWeak)) {
		// Remove reference.
		sym->m_numRefs--;

		// If a global symbol exists, re-link to it instead.
		if (model.m_gSyms.getSym(label.m_rawStr, sym)== RET_ERR_NONE) {
			// Symbol found- log re-link.
			string dbgStr = string("Relink '")     +
					        label.m_rawStr         +
							"' (to "               +
							sym->m_file            +
							"/"                    +
							to_string(sym->m_line) +
							")";
			Print::inst().log(LOG_DEBUG, label.m_file, label.m_line, dbgStr);
		}

		// (Re)apply reference.
		sym->m_numRefs++;
	}

	// Attempt link ONLY IF link hasn't been found.
	if (sym == nullptr) {
		// Search table for symbol to link to (MUST find for validity).
		if (model.m_gSyms.getSym(label.m_rawStr, sym)== RET_ERR_ERROR) {
			// No symbol found- log error.
			string errStr = string("Global no-def '") + label.m_rawStr + "'";
			Print::inst().log(LOG_ERROR, label.m_file, label.m_line, errStr);
			ModelUtil_recordError(model, RET_G_NODEF);
		}
		else {
			// Symbol now referenced- update counter.
			sym->m_numRefs++;

			// Log link.
			string dbgStr = string("Global link '") +
					        label.m_rawStr          +
							"' (to "                +
							sym->m_file             +
							"/"                     +
							to_string(sym->m_line)  +
							")";
			Print::inst().log(LOG_DEBUG, label.m_file, label.m_line, dbgStr);
		}
	}
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
