/*
 * StepParseCli.cpp: Step to process command line inputs into data model.
 */

#include "version.h"
#include "Common/Device/GetOpt.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/GetOptDefs.h"
#include "Common/Util/InfoUtil.h"
#include "Common/Util/StrUtil.h"
#include "Domain/CliFlag_e.h"
#include "Domain/RetCode_e.h"

#include "State/StepParseCli.h"

using namespace std;

//==============================================================================

// Definitions for creating new filename.
#define FILE_EXT_BIN   (".bin")

// Definition for easing exit condition checks during primary state process.
#define EXIT_PRGM (doExit || (model.m_summary.m_numErrs > 0))

// Definitions for info requests.
#define HELP_INFO string("Project RISCII C compiler (interface program)\n") + \
	                     "Usage: quaid.exe [options...] <input files...>\n" + \
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
						 "    -as <arg> add assembly input file\n"          + \
						 "    -E        pre-process only\n"                 + \
						 "    -S        pre-process + compile only\n"       + \
                         "    -I  <arg> add include directory path\n"       + \
						 "    -D  <arg> define variable for all files\n"    + \
						 "    -g        adds debug symbols to binary\n"     + \
						 "    -o  <arg> sets binary filename\n"             + \
						 "    -O1       do minor optimizations\n"           + \
						 "    -O2       do minor + major optimizations"
#define VERS_INFO string("quaid.exe ") + APP_VERSION

// Definitions for "string to LogType_e" conversions.
#define LEVEL_SILENT   ("silent")
#define LEVEL_ERROR    ("error")
#define LEVEL_WARNING  ("warning")
#define LEVEL_INFO     ("info")
#define LEVEL_DEBUG    ("debug")

//==============================================================================

// "Char set" used to verify cli-defined definitions.
string s_varLetter = "abcdefghijklmnopqrstuvqxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
string s_varNumber = "0123456789";

//==============================================================================
// Helper function to act as "string to flag enum" callback for GetOpt.
static int StepParseCli_asFlag(std::string argStr) {
	// Attempt to match string to known flag.
	AS_FLAG(argStr, "h",  CLI_FLAG_HELP);
	AS_FLAG(argStr, "v",  CLI_FLAG_VERSION);
	AS_FLAG(argStr, "ll", CLI_FLAG_LOG_LEVEL);
	AS_FLAG(argStr, "as", CLI_FLAG_ASM_FILE);
	AS_FLAG(argStr, "E",  CLI_FLAG_TO_CPP);
	AS_FLAG(argStr, "S",  CLI_FLAG_TO_ASM);
	AS_FLAG(argStr, "I",  CLI_FLAG_INC_DIR);
	AS_FLAG(argStr, "D",  CLI_FLAG_DEF);
	AS_FLAG(argStr, "g",  CLI_FLAG_DEBUG);
	AS_FLAG(argStr, "o",  CLI_FLAG_NAME);
	AS_FLAG(argStr, "O1", CLI_FLAG_O1);
	AS_FLAG(argStr, "O2", CLI_FLAG_O2);

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
// Helper function to ensure program has a valid output filename.
static void StepPareCli_rectifyOutFile(DataModel_t& model) {
	// Create output filename if none is given.
	if (model.m_outFile.size() == 0) {
		// Get name of the first input file.
		if (model.m_cFiles.size() == 0) {
			Terminate::inst().assert("rectify() no input");}
		string name = model.m_cFiles[0];

		// Replace file type.
		StrUtil_rmFtype(name);
		name += FILE_EXT_BIN;

		// Set the new name in the model.
		model.m_outFile = name;
	}
}

//==============================================================================
// Helper function to handle "add include directory" flag/command.
static void StepParseCli_handleDir(DataModel_t& model, string const& dir) {
	// Add to include directories.
	model.m_iDirs.push_back(dir);

	// Ensure input matches "directory" expectations.
	StrUtil_asDir(model.m_iDirs.back());
	if (model.m_iDirs.back().compare(dir) != 0) {
		// Required re-format = not a directory.
		Print::inst().cli(string("Bad directory '") + dir + "'");
		InfoUtil_recordError(model.m_summary, RET_BAD_ARG);
	}
}

//==============================================================================
// Helper function to handle "add include directory" flag/command.
static void StepParseCli_handleDef(DataModel_t& model, string const& def) {
	// Add definition for all files (allow repeats- cli rules > def rules).
	model.m_iDefs.push_back(def);

	// Verify the variable meets cli definition stds.
	bool badVar = false;
	for (size_t i = 0; i < def.size(); i++) {
		// Analyze.
		char chr = def[i];
		bool isLetter = (s_varLetter.find(chr) != string::npos);
		bool isNumber = (s_varNumber.find(chr) != string::npos);

		// Check (alphanumeric w/ letter prefix).
		if ((isLetter == false) && ((i == 0) | (isNumber == false))) {
			badVar = true;
			break;         // already bad- stop check
		}
	}

	// Error as needed.
	if (badVar) {
		Print::inst().cli(string("Bad definition '") + def + "'");
		InfoUtil_recordError(model.m_summary, RET_BAD_ARG);
	}
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
		case CLI_FLAG_ASM_FILE:
			model.m_sFiles.push_back(args.m_arg);
			break;
		case CLI_FLAG_TO_CPP:
			model.m_depth = PROC_TO_CPP;
			break;
		case CLI_FLAG_TO_ASM:
			model.m_depth = PROC_TO_ASM;
			break;
		case CLI_FLAG_INC_DIR:
			StepParseCli_handleDir(model, args.m_arg);
			break;
		case CLI_FLAG_DEF:
			StepParseCli_handleDef(model, args.m_arg);
			break;
		case CLI_FLAG_DEBUG:
			model.m_doDbg = true;
			break;
		case CLI_FLAG_NAME:
			model.m_outFile = args.m_arg;
			break;
		case CLI_FLAG_O1:
			model.m_optLvl = OPT_LVL_O1;
			break;
		case CLI_FLAG_O2:
			model.m_optLvl = OPT_LVL_O2;
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
				model.m_cFiles.push_back(args.m_value);
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
	if ((model.m_cFiles.size() == 0) && (EXIT_PRGM == false)) {
		Print::inst().cli("requires at least one input file");
		InfoUtil_recordError(model.m_summary, RET_NO_FILE);
	}

	// Analyze/report results (if everything if valid/correct).
	if (EXIT_PRGM == false) {
		// Ensure output name is resolved.
		StepPareCli_rectifyOutFile(model);

		// (Summarize results of cli parsing.)
		string numCFiles = string("    # C-files: ") +
				          to_string(model.m_cFiles.size());
		string numSFiles = string("    # S-files: ") +
				          to_string(model.m_sFiles.size());
		string depth     = string("    depth    : ") +
				           ((model.m_depth == PROC_TO_CPP) ? "i" :
				           (model.m_depth == PROC_TO_ASM) ? "s"  :
				        		                           "bin");
		string numDirs   = string("    # I-dirs : ") +
						         to_string(model.m_iDirs.size());
		string numDefs   = string("    # I-defs : ") +
						         to_string(model.m_iDefs.size());
		string debug     = string("    debug bin: ") +
				           ((model.m_doDbg) ? "true" : "false");
		string binName   = string("    bin name : ") + model.m_outFile;
		string optLvl    = string("    optimize : ") +
				           ((model.m_optLvl == OPT_LVL_O1) ? "1" :
				           (model.m_optLvl == OPT_LVL_O2) ? "2"  :
				        		                           "none");
		Print::inst().log(LOG_INFO, "=Cli Summary=");
		Print::inst().log(LOG_INFO, numCFiles);
		Print::inst().log(LOG_INFO, numSFiles);
		Print::inst().log(LOG_INFO, depth);
		Print::inst().log(LOG_INFO, numDirs);
		Print::inst().log(LOG_INFO, numDefs);
		Print::inst().log(LOG_INFO, debug);
		Print::inst().log(LOG_INFO, binName);
		Print::inst().log(LOG_INFO, optLvl);
	}

	// Exit program (vs step) as applicable.
	if (EXIT_PRGM) {Terminate::inst().silent(model.m_summary.m_retCode);}
}
