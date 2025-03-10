/*
 * StepCreateFile.cpp: TODO
 */

#include "Common/Device/File.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/InfoUtil.h"
#include "Common/Util/StrUtil.h"
#include "Domain/RetCode_e.h"
#include "Ds/IAstNode.h"
#include "State/StepCreateFile.h"

using namespace std;

//==============================================================================

// File extension type created by preprocessor.
#define EXT_TYPE ".i"

//==============================================================================
// TODO
void StepCreateFile_openWriteFile(DataModel_t& model, string& file) {
	// Derive output file's name.
	StrUtil_rmFtype(file);
	file += EXT_TYPE;

	// Open file.
	Print::inst().log(LOG_DEBUG, string("Opening '") + file + "'...");
	RetErr_e retErr = File::inst().open(file, FILE_OP_WRITE);

	// Record error on occurrence.
	if (retErr) {
		Print::inst().log(LOG_ERROR, string("Can't write '") + file + "'");
		InfoUtil_recordError(model.m_summary, RET_NO_WRIT);
	}
}


//==============================================================================
// TODO
void StepCreateFile_execute(DataModel_t& model, std::string& file) {
	// Ensure new file's "combo" of definition's don't conflict.
	model.m_defs.scopePush();
	model.m_srcAst->checkDefines(model);
	model.m_defs.scopePop();

	// Open file.
	if (model.m_summary.m_numErrs == 0) {
		StepCreateFile_openWriteFile(model, file);
	}

	// Write fully pre-processed text.
	if (model.m_summary.m_numErrs == 0) {
		model.m_srcAst->writeText(model);
	}

	// Close file.
	File::inst().close();

	// (Stay informative.)
	if (model.m_summary.m_numErrs == 0) {
		Print::inst().log(LOG_DEBUG, string("Finished '") + file + "'");
	}

	// Exit program (vs step) as applicable.
	if (model.m_summary.m_numErrs > 0) {
		Terminate::inst().summary(model.m_summary);
	}
}
