/*
 * GetOpt.cpp: Customized version of unistd.h's "getopt" function.
 */

#include "Common/Util/GetOptDefs.h"

#include "Common/Device/GetOpt.h"

using namespace std;

//==============================================================================
// Constructor- parses cli arguments into iterable list.
GetOpt::GetOpt(int const argc, char const* const argv[]) {
	// Process input tokens, skipping over "empty" ones.
	if (argv != nullptr) {
		for(int i = 0; i < argc; i++) {
			// Grab (non-nulls).
			string newStr = "";
			if (argv[i] != nullptr) {newStr += argv[i];}

			// Save (non-empties).
			if (newStr.size() > 0) {m_tokens.push_back(newStr);}
		}
	}
	m_nextIdx = 0; // begin with first one

	// Haven't processed yet- nothing to share.
	m_type     = CLI_TYPE_INVALID;
	m_flagCode = GETOPT_BAD_FLAG;
	m_value    = "";
	m_arg      = "";
}

//==============================================================================
// Iterates one step through arguments- updating public variables.
bool GetOpt::get(void) {
	// Reset output data.
	m_type     = CLI_TYPE_INVALID;
	m_flagCode = GETOPT_BAD_FLAG;
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
			m_flagCode   = asFlag(m_value);
			bool getArg  = needArg(m_flagCode);

			// Get argument if applicable.
			if ((getArg)                                   && // arg flag...
				(m_nextIdx < m_tokens.size())              && // w/ arg...
				(m_tokens[m_nextIdx][0] != FLAG_PREFIX)) {    // arg isn't flag
				m_arg = m_tokens[m_nextIdx];
				m_nextIdx++;
			}

			// Adjust for error cases.
			if (m_flagCode == GETOPT_BAD_FLAG)   {m_type = CLI_TYPE_UNKNOWN;}
			if ((getArg) && (m_arg.size() == 0)) {m_type = CLI_TYPE_NO_ARG;}
		}
	}

	// Return.
	return doParse;
}
