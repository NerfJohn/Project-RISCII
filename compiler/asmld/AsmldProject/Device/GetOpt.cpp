/*
 * GetOpt.cpp: Customized version of unistd.h's "getopt" function.
 */

#include "Device/GetOpt.h"

using namespace std;

//==============================================================================

// Prefix for all flag tokens.
#define FLAG_PREFIX '-'

// Flag options of asmld.exe.
#define FLAG_HELP    ("-h")
#define FLAG_VERSION ("-v")
#define FLAG_NAME    ("-o")
#define FLAG_LEVEL   ("-ll")

// (Yep, that's right- cheeky, targeted macros.)
#define FLAG(x)  (0 == flagStr.compare(x))
#define SET(x,y) {m_type = (x); m_asFlag = (y);}

//==============================================================================
// Helper function to parse asmld.exe specific flags/details.
void GetOpt::parseFlag(std::string const& flagStr) {
	// Set types as applicable.
	if      FLAG(FLAG_HELP)    SET(CLI_TYPE_LONE, CLI_FLAG_HELP)
	else if FLAG(FLAG_VERSION) SET(CLI_TYPE_LONE, CLI_FLAG_VERSION)
	else if FLAG(FLAG_NAME)    SET(CLI_TYPE_ARG,  CLI_FLAG_NAME)
	else if FLAG(FLAG_LEVEL)   SET(CLI_TYPE_ARG,  CLI_FLAG_LEVEL)
}

//==============================================================================
// Constructor- inits parse by taking in pre-separated tokens.
GetOpt::GetOpt(int const argc, char const * const argv[]) {
	// Save tokens for parsing in-between calls.
	for (int i = 0; i < argc; i++) {
		// "Fill-in" nulls with empty strings.
		string newStr = "";
		if ((argv != nullptr) && (argv[i] != nullptr)) {newStr = argv[i];}

		// Save non-empty strings.
		if (newStr.size() > 0) {m_tokens.push_back(newStr);}
	}

	// Haven't processed yet- nothing to share.
	m_asFlag = CLI_FLAG_INVALID;
	m_type   = CLI_TYPE_INVALID;
	m_value  = "";
	m_arg    = "";

	// Init internal metrics of parsing.
	m_nextIdx = 0;
}

//==============================================================================
// Parses one token/flag-argument pair into shared data.
bool GetOpt::getOpt(void) {
	// Reset output data.
	m_asFlag = CLI_FLAG_INVALID;
	m_type   = CLI_TYPE_INVALID;
	m_value  = "";
	m_arg    = "";

	// Save "state" of operation prior to altering needed data.
	bool doParse = (m_nextIdx < m_tokens.size());

	// Process if not finished yet.
	if (doParse) {
		// Set/consume next token.
		m_value = m_tokens[m_nextIdx];
		m_nextIdx++;

		// Determine type- parsing deeper through flags as needed.
		if (m_value[0] == FLAG_PREFIX) {parseFlag(m_value);}
		else                           {m_type = CLI_TYPE_FILE;}

		// Set/consume argument for appropriate flags.
		if ((m_type == CLI_TYPE_ARG)                && // arg flag...
			(m_nextIdx < m_tokens.size())           && // with argument...
			(m_tokens[m_nextIdx][0] != FLAG_PREFIX)) { // that's not a flag
			m_arg = m_tokens[m_nextIdx];
			m_nextIdx++;
		}
	}

	// Return.
	return doParse;
}
