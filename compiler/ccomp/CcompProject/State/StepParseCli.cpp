/*
 * StepParseCli.cpp: Step to process command line inputs into data model.
 */

#include "version.h"
#include "Common/Device/GetOpt.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/GetOptDefs.h"
#include "Common/Util/InfoUtil.h"
#include "Domain/CliFlag_e.h"
#include "Domain/RetCode_e.h"

#include "State/StepParseCli.h"

using namespace std;

//==============================================================================

// Definition for easing exit condition checks during primary state process.
#define EXIT_PRGM (doExit || (model.m_summary.m_numErrs > 0))

// Definitions for info requests.
#define HELP_INFO string("Project RISCII C Compiler Core (C -> Asm)\n")     + \
	                     "Usage: ccomp.exe [options...] <input files...>\n" + \
						 "\n"                                               + \
						 "Options:\n"                                       + \
						 "    -h        print this menu and exit\n"         + \
						 "    -v        print version info and exit\n"      + \
						 "    -ll <arg> sets stdout log level to:\n"        + \
						 "                  silent  no log info\n"          + \
						 "                  error   only errors\n"          + \
						 "              (d) warning errors and warnings\n"  + \
						 "                  info    process related info\n" + \
						 "                  debug   all available output\n" + \
						 "    -O1       perform general optimizations"
#define VERS_INFO string("ccomp.exe ") + APP_VERSION

// Definitions for "string to LogType_e" conversions.
#define LEVEL_SILENT   ("silent")
#define LEVEL_ERROR    ("error")
#define LEVEL_WARNING  ("warning")
#define LEVEL_INFO     ("info")
#define LEVEL_DEBUG    ("debug")

//==============================================================================
// Helper function to act as "string to flag enum" callback for GetOpt.
static int StepParseCli_asFlag(std::string argStr) {
	// Attempt to match string to known flag.
	AS_FLAG(argStr, "h",  CLI_FLAG_HELP);
	AS_FLAG(argStr, "v",  CLI_FLAG_VERSION);
	AS_FLAG(argStr, "ll", CLI_FLAG_LOG_LEVEL);
	AS_FLAG(argStr, "O1", CLI_FLAG_OPTIMIZE);

	// Otherwise, it's not a flag.
	return CLI_FLAG_INVALID;
}

//==============================================================================
// Helper function to act as "arg required for flag enum" callback for GetOpt.
static bool StepParseCli_needArg(int flagCode) {
	// All "arg flags" are under a set limit.
	return (flagCode <= CLI_FLAG_ARG_MAX);
}

//==============================================================================
// Helper function to handle "set log level" flag/command.
static void StepParseCli_handleLevel(DataModel_t&       model,
		                             std::string const& level) {
	// Determine level from string.
	LogType_e newLvl = LOG_SILENT;
	if      (level.compare(LEVEL_SILENT) == 0)  {newLvl = LOG_SILENT;}
	else if (level.compare(LEVEL_ERROR) == 0)   {newLvl = LOG_ERROR;}
	else if (level.compare(LEVEL_WARNING) == 0) {newLvl = LOG_WARNING;}
	else if (level.compare(LEVEL_INFO) == 0)    {newLvl = LOG_INFO;}
	else if (level.compare(LEVEL_DEBUG) == 0)   {newLvl = LOG_DEBUG;}
	else {
		// Not a known log level.
		Print::inst().cli(string("unknown log level '") + level + "'");
		InfoUtil_recordError(model.m_summary, RET_BAD_ARG);
	}

	// Set log level.
	Print::inst().setLogLevel(newLvl);
}

//==============================================================================
// Helper function to handle parsed flag options.
static void StepParseCli_handleFlag(DataModel_t&  model,
		                            GetOpt const& args,
									bool&         doExit) {
	// Handle based on specific flag.
	switch(args.m_flagCode) {
		case CLI_FLAG_HELP:
			Print::inst().cli(HELP_INFO);
			doExit = true;
			break;
		case CLI_FLAG_VERSION:
			Print::inst().cli(VERS_INFO);
			doExit = true;
			break;
		case CLI_FLAG_LOG_LEVEL:
			StepParseCli_handleLevel(model, args.m_arg);
			break;
		case CLI_FLAG_OPTIMIZE:
			model.m_doOpt = true;
			break;
		default:
			// Unknown flag? GetOpt/callback error- bug!
			Terminate::inst().assert("handleFlag() unknown flag");
	}
}

//==============================================================================
// Check/parses arguments from command line into data model.
void StepParseCli_execute(DataModel_t&      model,
		                  int const         argc,
						  char const* const argv[]) {
	// Track "exit on handle" flags.
	bool doExit = false;

	// Prep parsing of cli arguments.
	OptInit_t initData = {
		.m_argc    = argc,
		.m_argv    = argv,
		.m_asFlag  = StepParseCli_asFlag,
		.m_needArg = StepParseCli_needArg,
		.m_badFlag = CLI_FLAG_INVALID
	};
	GetOpt args(initData);
	args.getOpt();                    // ignore program's exe path/name

	// Parse each cli argument.
	while(args.getOpt()) {
		// Handle argument appropriately.
		switch(args.m_type) {
			case CLI_TYPE_FILE:
				model.m_files.push_back(args.m_value);
				break;
			case CLI_TYPE_FLAG:
				StepParseCli_handleFlag(model, args, doExit);
				break;
			case CLI_TYPE_UNKNOWN:
				Print::inst().cli(string("Unknown '") + args.m_value + "'");
				InfoUtil_recordError(model.m_summary, RET_NOT_CLI);
				break;
			case CLI_TYPE_NO_ARG:
				Print::inst().cli(args.m_value + " requires an argument");
				InfoUtil_recordError(model.m_summary, RET_FNO_ARG);
				break;
			default:
				// Bad parsing by GetOpt- bug!
				Terminate::inst().assert("getOpt() bad type");
		}

		// Break out for errors or other exit scenario.
		if (EXIT_PRGM) {break;}
	}

	// Ensure at least some input to work with.
	if ((model.m_files.size() == 0) && (EXIT_PRGM == false)) {
		Print::inst().cli("requires at least one input file");
		InfoUtil_recordError(model.m_summary, RET_NO_FILE);
	}

	// Analyze/report results (if everything if valid/correct).
	if (EXIT_PRGM == false) {
		// (Summarize results of cli parsing.)
		string numFiles = string("    # files : ") +
				          to_string(model.m_files.size());
		string optStr   = string("    optimize: ") + ((model.m_doOpt) ?
				          "true" : "false");
		Print::inst().log(LOG_INFO, "=Cli Summary=");
		Print::inst().log(LOG_INFO, numFiles);
		Print::inst().log(LOG_INFO, optStr);
	}

	// Exit program (vs step) as applicable.
	if (EXIT_PRGM) {Terminate::inst().silent(model.m_summary.m_retCode);}
}
