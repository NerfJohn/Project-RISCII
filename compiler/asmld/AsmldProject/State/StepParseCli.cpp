/*
 * StepParseCli.cpp: State/step to process command line inputs into data model.
 */

#include "version.h"
#include "Device/GetOpt.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/ModelUtil.h"

#include "State/StepParseCli.h"

using namespace std;

//==============================================================================

// Definitions for creating new filename.
#define FILE_DOT       ('.')
#define FILE_EXT_BIN   ("bin")

// Definitions for setting log level.
#define LOG_LEVEL(x,y) (level.compare(x) == 0) {Print::inst().setLogLevel(y);}
#define LEVEL_SILENT   ("silent")
#define LEVEL_ERROR    ("error")
#define LEVEL_WARNING  ("warning")
#define LEVEL_INFO     ("info")
#define LEVEL_DEBUG    ("debug")

// Common definition for breaking out/returning.
#define DO_EXIT        ((model.m_numErrs > 0) || doHelp || doVer)

//==============================================================================
// Helper function to ensure program has a valid output filename.
static void StepPareCli_rectifyOutFile(DataModel_t& model) {
	// Create output filename if none is given.
	if (model.m_outFile.size() == 0) {
		// Get name of the first input file.
		if (model.m_files.size() == 0) {Terminate_assert("rectify() no input");}
		string name = model.m_files[0];

		// Extract the root name.
		size_t idx = name.find(FILE_DOT);
		if (idx < name.size()) {name = name.substr(0, ++idx);}

		// Add the appropriate file extension.
		name += FILE_EXT_BIN;

		// Set the new name in the model.
		model.m_outFile = name;
	}
}

//==============================================================================
// Helper function to handle help menu flag/request.
static void StepParseCli_handleHelp(bool& doHelp) {
	// Print help menu.
	Print::inst().cli("Assembler/Linker program for Project RISCII hardware");
	Print::inst().cli("Usage: asmld.exe [options...] <input files...>");
	Print::inst().cli("");
	Print::inst().cli("Options:");
	Print::inst().cli("    -h        print this menu and exit");
	Print::inst().cli("    -v        print version info and exit");
	Print::inst().cli("    -o  <arg> sets filename to use for output file");
	Print::inst().cli("    -ll <arg> sets stdout log level to:");
	Print::inst().cli("                  silent  no log output");
	Print::inst().cli("                  error   only errors");
	Print::inst().cli("                  warning errors/warnings (default)");
	Print::inst().cli("                  info    process related info");
	Print::inst().cli("                  debug   all available output");

	// Mark that help menu has been requested.
	doHelp = true;
}

//==============================================================================
// Helper function to handle version info flag/request.
static void StepParseCli_handleVersion(bool& doVer) {
	// Print version info.
	Print::inst().cli(string("asmld.exe ") + APP_VERSION);

	// Mark that version has been requested.
	doVer = true;
}

//==============================================================================
// Helper function to handle setting the log level.
static void StepParseCli_handleLevel(DataModel_t& model, string const& level) {
	// Set level based on given argument.
	if      LOG_LEVEL(LEVEL_SILENT,  LOG_SILENT)
	else if LOG_LEVEL(LEVEL_ERROR,   LOG_ERROR)
	else if LOG_LEVEL(LEVEL_WARNING, LOG_WARNING)
	else if LOG_LEVEL(LEVEL_INFO,    LOG_INFO)
	else if LOG_LEVEL(LEVEL_DEBUG,   LOG_DEBUG)
	else {
		// Unknown level- report to user.
		Print::inst().cli(string("unknown log level '") + level + "'");
		ModelUtil_recordError(model, RET_BAD_ARG);
	}
}

//==============================================================================
// Executes process to check/parse arguments from command line into data model.
void StepParseCli_execute(DataModel_t& model,
		                  int const argc,
						  char const* const argv[]) {
	// Vars to track requested info.
	bool doHelp = false;
	bool doVer  = false;

	// Get cli arguments in parse-able means.
	GetOpt args(argc, argv);
	args.getOpt();           // first arg is program file- discard

	// Parse each cli argument.
	while(args.getOpt()) {
		// (Sanity check type.)
		if (args.m_type == CLI_TYPE_INVALID) {
			Terminate_assert("handleCli() unknown type");
		}

		// Files are saved for reading/parsing later (1/3 IF).
		if (args.m_type == CLI_TYPE_FILE) {
			model.m_files.push_back(args.m_value);
		}

		// Flags with arguments MUST have the argument (2/3 ELSE IF).
		else if ((args.m_type == CLI_TYPE_ARG) && (args.m_arg.size() == 0)) {
			Print::inst().cli(args.m_value + " requires an argument");
			ModelUtil_recordError(model, RET_FNO_ARG);
		}

		// Handle specific flags (3/3 ELSE IF).
		else if ((args.m_type == CLI_TYPE_LONE) ||
				 (args.m_type == CLI_TYPE_ARG)) {
			switch (args.m_asFlag) {
				case CLI_FLAG_HELP:
					StepParseCli_handleHelp(doHelp);
					break;
				case CLI_FLAG_VERSION:
					StepParseCli_handleVersion(doVer);
					break;
				case CLI_FLAG_NAME:
					model.m_outFile = args.m_arg;
					break;
				case CLI_FLAG_LEVEL:
					StepParseCli_handleLevel(model, args.m_arg);
					break;
				default:
					// Unknown flag? Bug!
					Terminate_assert("handleCli() with unknown flag");
			}
		}

		// If error (or terminal flag) was hit, break out to return.
		if (DO_EXIT) {break;}
	}

	// Ensure input was given (unless error/terminal was already hit).
	if ((model.m_files.size() == 0) && (DO_EXIT == false)) {
		Print::inst().cli("requires at least one input file");
		ModelUtil_recordError(model, RET_NO_FILE);
	}

	// Exit as applicable.
	if (DO_EXIT) {Terminate_silent(model.m_retCode);}

	// Ensure output filename is valid.
	StepPareCli_rectifyOutFile(model);

	// (Summarize results of cli parsing.)
	string outFile  = string("    Output:  ") + model.m_outFile;
	string numFiles = string("    # files: ") + to_string(model.m_files.size());
	Print::inst().log(LOG_INFO, "=Cli Summary=");
	Print::inst().log(LOG_INFO, outFile);
	Print::inst().log(LOG_INFO, numFiles);
}

