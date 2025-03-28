/*
 * StepDoProcedure.cpp: Step to conduct main procedure/calls of program.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/InfoUtil.h"
#include "Common/Util/StrUtil.h"
#include "Device/Cmd.h"
#include "Domain/RetCode_e.h"

#include "State/StepDoProcedure.h"

using namespace std;

//==============================================================================

// Definition for easing exit condition checks during primary state process.
#define EXIT_PRGM (model.m_summary.m_numErrs > 0)

// Definitions for expected locations of sub-programs.
#define CPREP_EXE (string("./cprep.exe"))
#define CCOMP_EXE (string("./ccomp.exe"))
#define ASMLD_EXE (string("./asmld.exe"))

// Definitions for expected file extensions.
#define I_EXT (".i")
#define S_EXT (".s")

// Definition for expected values.
#define CALL_SUCCESS 0

//==============================================================================
// Helper function to add files (with common extension) to command.
static void StepDoProcedure_addFiles(string&         cmd,
		                             vector<string>& files,
									 string const    ext) {
	// Append each file to the command.
	for (string fname : files) {
		// Determine new name (as applicable).
		if (ext.size() > 0) {
			StrUtil_rmFtype(fname);
			fname += ext;
		}

		// Append.
		cmd += string(" ") + fname;
	}
}

//==============================================================================
// Runs main series of command line calls to compile program.
void StepDoProcedure_execute(DataModel_t& model) {
	// String-ify the passed log level.
	string logArg = " -ll ";
	switch (model.m_logLvl) {
		case LOG_SILENT:  logArg += "silent";  break;
		case LOG_ERROR:   logArg += "error";   break;
		case LOG_WARNING: logArg += "warning"; break;
		case LOG_INFO:    logArg += "warning"; break; // quaid.exe info only
		case LOG_DEBUG:   logArg += "info";    break; // step up from previous
		default: Terminate::inst().assert("strLog() bad arg"); break;
	}

	// Run pre-processor on all input files (asm included).
	string cprepCmd = CPREP_EXE;
	for (string inc : model.m_iDirs) {cprepCmd += string(" -I ") + inc;}
	for (string def : model.m_iDefs) {cprepCmd += string(" -D ") + def;}
	cprepCmd += logArg;
	StepDoProcedure_addFiles(cprepCmd, model.m_cFiles, "");
	StepDoProcedure_addFiles(cprepCmd, model.m_sFiles, "");
	Print::inst().log(LOG_INFO, string("Running '") + cprepCmd + "'");
	if (Cmd_run(cprepCmd) != CALL_SUCCESS) {
		Print::inst().log(LOG_ERROR, "Errors while pre-processing");
		InfoUtil_recordError(model.m_summary, RET_ERR_CPP);
	}

	// Run core compiler on c-files (as applicable).
	bool ccompSkip = EXIT_PRGM                     |
			         (model.m_cFiles.size() == 0)  |
					 (model.m_depth == PROC_TO_CPP);
	if (ccompSkip == false) {
		string ccompCmd = CCOMP_EXE;
		if (model.m_optLvl == OPT_LVL_O2) {ccompCmd += " -O1";}
		ccompCmd += logArg;
		StepDoProcedure_addFiles(ccompCmd, model.m_cFiles, I_EXT);
		Print::inst().log(LOG_INFO, string("Running '") + ccompCmd + "'");
		if (Cmd_run(ccompCmd) != CALL_SUCCESS) {
			Print::inst().log(LOG_ERROR, "Errors while compiling");
			InfoUtil_recordError(model.m_summary, RET_ERR_CMP);
		}
	}

	// Run assembler/linker on all s-files (as applicable).
	bool asmldSkip = EXIT_PRGM | (model.m_depth != PROC_TO_BIN);
	if (asmldSkip == false) {
		string asmldCmd = ASMLD_EXE;
		if (model.m_doDbg)                {asmldCmd += " -g";}
		if (model.m_optLvl >= OPT_LVL_O1) {asmldCmd += " -r";}
		asmldCmd += string(" -o ") + model.m_outFile;
		asmldCmd += logArg;
		StepDoProcedure_addFiles(asmldCmd, model.m_cFiles, S_EXT);
		StepDoProcedure_addFiles(asmldCmd, model.m_sFiles, I_EXT);
		Print::inst().log(LOG_INFO, string("Running '") + asmldCmd + "'");
		if (Cmd_run(asmldCmd) != CALL_SUCCESS) {
			Print::inst().log(LOG_ERROR, "Errors while assembling");
			InfoUtil_recordError(model.m_summary, RET_ERR_ASM);
		}
	}

	// Exit program (vs step) as applicable.
	if (EXIT_PRGM) {Terminate::inst().summary(model.m_summary);}
}
