/*
 * DsUtil.cpp: Utilities regarding node/data structure operations.
 */

#include "Common/Util/Msg.h"
#include "Util/AppUtil.h"

#include "Util/DsUtil.h"

using namespace std;

//==============================================================================
// Add symbol at current scope. (Re-)link if already strongly declared.
void DsUtil_addSym(DataModel_t& model, std::string& name, Ptr<Sym_t>& newSym) {
	// Attempt Add.
	RetErr_e res = model.m_syms.addLocal(name, newSym);
	if (res == RET_ERR_NONE) {model.m_symCnt++;}

	// Check current.
	Ptr<Sym_t> curSym;
	model.m_syms.get(name, curSym);
	bool oldSym = (res == RET_ERR_ERROR);
	bool oldDef = (oldSym && curSym->m_isDefined);

	// Handle cases 1) one definition 3) two definitions.
	if (oldDef ^ newSym->m_isDefined) {
		curSym->m_isDefined = true;
	}
	else if (oldDef & newSym->m_isDefined) {
		// Report.
		string err = Msg() + "Redefined '"  +
				             name           +
							 "' (also at "  +
							 curSym->m_file +
							 ":"            +
							 curSym->m_line +
							 ")";
		Print_log(LOG_ERROR, newSym->m_file, newSym->m_line, err);
		AppUtil_error(model, RET_MUL_DEF);
	}

	// Reuse current symbol (as applicable).
	if (oldSym) {
		// Inform.
		string dbg = Msg() + "Reused symbol '"  +
				             name               +
							 "' (also at "      +
							 curSym->m_file     +
							 ":"                +
							 curSym->m_line     +
							 ")";
		Print_log(LOG_DEBUG, newSym->m_file, newSym->m_line, dbg);

		// Reuse.
		newSym = curSym;
	}

	// (Stay Informative.)
	if (oldSym == false) {
		string dbg = Msg() + "Added symbol '" + name + "'";
		Print_log(LOG_DEBUG, newSym->m_file, newSym->m_line, dbg);
	}
}
