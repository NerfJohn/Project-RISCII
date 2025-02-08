/*
 * GetOpt.cpp: Customized version of unistd.h's "getopt" function.
 */

#include "Common/Util/GetOptDefs.h"

#include "Common/Device/GetOpt.h"

using namespace std;

//==============================================================================
// Constructor- preps to parse given arguments (with flag finder).
GetOpt::GetOpt(int const          argc, 
	           char const * const argv[],
	           CliType_e         (asFlag*)(std::string)) {
	// Save tokens for parsing in-between calls.
	for (int i = 0; i < argc; i++) {
		// "Fill-in" nulls with empty strings.
		string newStr = "";
		if ((argv != nullptr) && (argv[i] != nullptr)) {newStr = argv[i];}

		// Save non-empty strings.
		if (newStr.size() > 0) {m_tokens.push_back(newStr);}
	}

	// Haven't processed yet- nothing to share.
	m_type   = CLI_INVALID;
	m_value  = "";
	m_arg    = "";

	// Init internal metrics of parsing.
	m_nextIdx = 0;
	m_asFlag  = asFlag;
}

//==============================================================================
// Parses one token/flag-argument pair into shared data.
bool GetOpt::getOpt(void) {
	// Reset output data.
	m_type   = CLI_INVALID;
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
		if (m_value[0] == FLAG_PREFIX) {asFlag(m_value);}
		else                           {m_type = CLI_FILE;}

		// Set/consume argument for appropriate flags.
		if ((m_type <= CLI_FLAG_ARG_MAX)            && // arg flag...
			(m_nextIdx < m_tokens.size())           && // with argument...
			(m_tokens[m_nextIdx][0] != FLAG_PREFIX)) { // that's not a flag
			m_arg = m_tokens[m_nextIdx];
			m_nextIdx++;
		}
		
		// Recognize "arg flag with no arg" scenarios.
		if ((m_type <= CLI_FLAG_ARG_MAX) && (m_arg.size() == 0)) {
			m_type = CLI_NO_ARG;
		}
	}

	// Return.
	return doParse;
}
