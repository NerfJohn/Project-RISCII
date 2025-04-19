/*
 * StepParseCli.cpp: Step to process command line inputs into data model.
 */

#include "version.h"
#include "Common/Device/GetOpt.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/Msg.h"
#include "Common/Util/StrUtil.h"
#include "Domain/CliFlag_e.h"
#include "Util/AppUtil.h"

#include "State/StepParseCli.h"

using namespace std;

//==============================================================================

// Definitions for info requests.
#define HELP_INFO string("Project RISCII C Compiler Core (C -> Asm)")   + LF + \
	                     "Usage: ccomp.exe [options...] <files...>"     + LF + \
						 ""                                             + LF + \
						 "Options:"                                     + LF + \
						 "    -h        print this menu and exit"       + LF + \
						 "    -v        print program version and exit" + LF + \
						 "    -ll <arg> set stdout log level to:"       + LF + \
						 "                  silent  no log info"        + LF + \
						 "                  error   only errors"        + LF + \
						 "              (d) warning errors/warnings"    + LF + \
						 "                  info    process info"       + LF + \
						 "                  debug   all output"         + LF + \
						 "    -O1       perform optimizations"
#define VERS_INFO string("ccomp.exe ") + APP_VERSION

// (Targeted) macro to ease determine when to "skip to exit".
#define EXIT_PRGM (doExit || (model.m_numErrs > 0))

//==============================================================================
// User-specific functions GetOpt relies on for parsing.
int asFlag(std::string s) {
	// Attempt to match to a flag.
	AS_FLAG("h",  CLI_FLAG_HELP);
	AS_FLAG("v",  CLI_FLAG_VERSION);
	AS_FLAG("ll", CLI_FLAG_LOG_LEVEL);
	AS_FLAG("O1", CLI_FLAG_O1);

	// Otherwise, bad flag.
	return GETOPT_BAD_FLAG;
}

//==============================================================================
// User-specific functions GetOpt relies on for parsing.
bool needArg(int flgCode) {
	// All "arg flags" are under a set limit.
	return (flgCode >= CLI_FLAG_ARG_MIN);
}

//==============================================================================
// Helper function to handle parsed flag options.
static void StepParseCli_handleFlag(DataModel_t&  model,
		                            GetOpt const& args,
									bool&         doExit) {
	// (Intermediate vars of switch cases.)
	string    str;
	LogType_e resType;

	// Handle based on specific flag.
	switch(args.m_flagCode) {
		case CLI_FLAG_HELP:
			Print_cli(HELP_INFO);
			doExit = true;
			break;
		case CLI_FLAG_VERSION:
			Print_cli(VERS_INFO);
			doExit = true;
			break;
		case CLI_FLAG_LOG_LEVEL:
			// Translate/verify log level.
			if (StrUtil_asLogType(resType, args.m_arg) == RET_ERR_ERROR) {
				str = Msg() + "unknown log level '" + args.m_arg + "'";
				Print_cli(str);
				AppUtil_error(model, RET_BAD_ARG);
			}
			else {Print_setLogLevel(resType);}
			break;
		case CLI_FLAG_O1:
			model.m_doOpt = true;
			break;
		default:
			// Unknown flag? GetOpt/callback error- bug!
			AppUtil_exitBug("handleFlag() unknown flag");
	}
}

//==============================================================================
// Check/parses arguments from command line into data model.
void StepParseCli_execute(DataModel_t& model, int argc, char* argv[]) {
	// Track "exit on handle" flags.
	bool doExit = false;

	// Prep parsing of cli arguments.
	GetOpt args(argc, argv);
	args.get();              // ignore path to program exe

	// Parse each cli argument.
	while(args.get()) {
		// Handle argument appropriately.
		switch(args.m_type) {
			case CLI_TYPE_FILE:
				model.m_files.push_back(args.m_value);
				break;
			case CLI_TYPE_FLAG:
				StepParseCli_handleFlag(model, args, doExit);
				break;
			case CLI_TYPE_UNKNOWN:
				Print_cli(string("Unknown '") + args.m_value + "'");
				AppUtil_error(model, RET_NOT_CLI);
				break;
			case CLI_TYPE_NO_ARG:
				Print_cli(args.m_value + " requires an argument");
				AppUtil_error(model, RET_FNO_ARG);
				break;
			default:
				// Bad parsing by GetOpt- bug!
				AppUtil_exitBug("getOpt() bad type");
		}

		// Break out for errors or other exit scenario.
		if (EXIT_PRGM) {break;}
	}

	// Ensure at least some input to work with.
	if ((model.m_files.size() == 0) && (EXIT_PRGM == false)) {
		Print_cli("requires at least one input file");
		AppUtil_error(model, RET_NO_FILE);
	}

	// Analyze/report results (if everything if valid/correct).
	if (EXIT_PRGM == false) {
		// (Summarize results of cli parsing.)
		Print_log(LOG_INFO, Msg() + "=Cli Summary=");
		Print_log(LOG_INFO, Msg() + "    # files : " + model.m_files.size());
		Print_log(LOG_INFO, Msg() + "    optimize: " + model.m_doOpt);
	}

	// Exit program (vs step) as applicable.
	if (EXIT_PRGM) {Terminate_silent(model.m_exitCode);}
}
