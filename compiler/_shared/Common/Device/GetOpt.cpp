/*
 * GetOpt.cpp: Customized version of unistd.h's "getopt" function.
 */

#include "Common/Device/Terminate.h"
#include "Common/Util/GetOptDefs.h"

#include "Common/Device/GetOpt.h"

using namespace std;

//==============================================================================
// Constructor- inits cli parsing for given input data.
GetOpt::GetOpt(OptInit_t const& initData) {
	// Save tokens for parsing in-between calls.
	for (int i = 0; i < initData.m_argc; i++) {
		// "Fill-in" nulls with empty strings.
		string newStr = "";
		if ((initData.m_argv != nullptr) && (initData.m_argv[i] != nullptr)) {
			newStr = initData.m_argv[i];
		}

		// Save non-empty strings.
		if (newStr.size() > 0) {m_tokens.push_back(newStr);}
	}

	// Save callback/handler functions.
	IF_NULL(initData.m_asFlag, "GetOpt() with null asFlag()");
	IF_NULL(initData.m_needArg, "GetOpt() with null needArg()");
	m_asFlag  = initData.m_asFlag;
	m_needArg = initData.m_needArg;

	// Save special values.
	m_badFlag = initData.m_badFlag;

	// Haven't processed yet- nothing to share.
	m_type     = CLI_TYPE_INVALID;
	m_flagCode = m_badFlag;
	m_value    = "";
	m_arg      = "";

	// Init internal metrics of parsing.
	m_nextIdx = 0;
}

//==============================================================================
// Parses one token/flag-argument pair into shared data.
bool GetOpt::getOpt(void) {
	// Reset output data.
	m_type     = CLI_TYPE_INVALID;
	m_flagCode = m_badFlag;
	m_value    = "";
	m_arg      = "";

	// Save "state" of operation prior to altering needed data.
	bool doParse = (m_nextIdx < m_tokens.size());

	// Process if not finished yet.
	if (doParse) {
		// Set/consume next token.
		m_value = m_tokens[m_nextIdx];
		m_nextIdx++;

		// Determine overall type.
		m_type = (m_value[0] == FLAG_PREFIX) ? CLI_TYPE_FLAG : CLI_TYPE_FILE;

		// Dig into specifics details/errors if a flag.
		if (m_type == CLI_TYPE_FLAG) {
			// Get more detail on flag.
			m_flagCode   = m_asFlag(m_value);
			bool getArg  = m_needArg(m_flagCode);

			// Get argument if applicable.
			if ((getArg)                                   && // arg flag...
				(m_nextIdx < m_tokens.size())              && // w/ arg...
				(m_tokens[m_nextIdx][0] != FLAG_PREFIX)) {    // that's no flag
				m_arg = m_tokens[m_nextIdx];
				m_nextIdx++;
			}

			// Adjust for error cases.
			if (m_flagCode == m_badFlag)         {m_type = CLI_TYPE_UNKNOWN;}
			if ((getArg) && (m_arg.size() == 0)) {m_type = CLI_TYPE_NO_ARG;}
		}
	}

	// Return.
	return doParse;
}
