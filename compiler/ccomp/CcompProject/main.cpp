/*
 * main.cpp: Head of the program/application.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/Msg.h"
#include "Domain/DataModel_t.h"
#include "State/StepLexFile.h"
#include "State/StepParseCli.h"
#include "State/StepParseFile.h"
#include "Util/AppUtil.h"

using namespace std;


// TODO
#include "Device/AsmWriter.h"
void foo(void) {
	string fname = "out.s";
	AsmWriter asmGen(fname);

	string myAddr = "myAddr";
	string label  = "0r";
	deque<int32_t> qq = {4,5,6};

	if (RET_ERR_ERROR == asmGen.labelLocal(myAddr)) {Print_cli("err0");}
	if (RET_ERR_ERROR == asmGen.labelGlobal(myAddr)) {Print_cli("err1");}
	if (RET_ERR_ERROR == asmGen.dataBss(23)) {Print_cli("err2");}
	if (RET_ERR_ERROR == asmGen.dataInit(qq)) {Print_cli("err3");}
	if (RET_ERR_ERROR == asmGen.loadLit(ASMWRITER_REG_L0, 127)) {Print_cli("err4");}
	if (RET_ERR_ERROR == asmGen.loadLit(ASMWRITER_REG_FP, 128)) {Print_cli("err5");}
	if (RET_ERR_ERROR == asmGen.loadLit(ASMWRITER_REG_SP, myAddr)) {Print_cli("err6");}
	if (RET_ERR_ERROR == asmGen.loadVal(ASMWRITER_REG_L1, ASMWRITER_REG_L2, -3)) {Print_cli("err7");}

	Terminate_silent(RET_SUCCESS);
}

//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {
	// Init devices.
	Terminate_setBugCode(RET_ASSERT);
	Print_setLogLevel(LOG_WARNING);   // default = log errors/warnings
	Print_setLogName("ccomp.exe");    // Set to name of program

	// Init program's data.
	DataModel_t model = {
		// General Summary/Progress.
		.m_numErrs  = 0,
		.m_numWarns = 0,
		.m_exitCode = RET_SUCCESS,               // start optimistic

		// Parsed Cli Results.
		.m_files = {},
		.m_doOpt = false,                        // default = no optimization

		// AST Processing Artifacts.
		.m_tkns   = {},
		.m_ast    = Ptr<IAstNode>(nullptr),
		.m_syms   = {},
		.m_fSym   = Ptr<Sym_t>(nullptr),
		.m_symCnt = 0
	};

	foo();

	// Collect inputs.
	bool cliExit = StepParseCli_execute(model, argc, argv);

	// Compile each file.
	if (false == cliExit) {
		Print_log(LOG_INFO, "=Process=");
		for (string file : model.m_files) {
			// Prep/cleanup.
			model.m_tkns   = {};
			model.m_ast    = Ptr<IAstNode>(nullptr);
			while (model.m_syms.scopeNum() > 0) {model.m_syms.scopePop();}
			model.m_fSym   = Ptr<Sym_t>(nullptr);
			model.m_symCnt = 0;

			// Compile.
			Print_log(LOG_INFO, Msg() + "Compiling '" + file + "'...");
			if (APP_OK) {StepLexFile_execute(model, file);}
			if (APP_OK) {StepParseFile_execute(model);}
			if (APP_OK) {model.m_ast->analyze(model);}

			// Breakout for errors.
			if (APP_OK == false) {break;}
		}
	}

	// Exit with summary (nix cli- no log level resolution).
	if (cliExit) {Terminate_silent(model.m_exitCode);}
	else         {AppUtil_exitSummary(model);}
	return RET_SUCCESS; // Not reached- appeasing g++
}
