/*
 * GetOpt.h: Customized version of unistd.h's "getopt" function.
 */

#ifndef DEVICE_GETOPT_H_
#define DEVICE_GETOPT_H_

#include <string>
#include <vector>
#include "Domain/CliFlag_e.h"
#include "Domain/CliType_e.h"

/*
 * Enhanced version of "getopt" function (tailored for RISCII compiler use).
 * Instances can parse through command line input to extract file and flag info
 * in an iterable manner.
 */
class GetOpt {
public:
	// Parsed data shared with the user.
	CliFlag_e   m_asFlag; // specific flag encountered (as applicable)
	CliType_e   m_type;   // type of token
	std::string m_value;  // raw value of token
	std::string m_arg;    // argument of flag (as applicable, empty if none)

	/*
	 * Constructor- inits parse by taking in pre-separated tokens.
	 *
	 * Takes in tokens akin to main function parameters. Saves local copy
	 * of tokens (ignoring null and empty strings). Function prepares to parse,
	 * but does not begin the process.
	 *
	 * @param argc number of tokens in argv
	 * @param argv array of string tokens to parse
	 */
	GetOpt(int const argc, char const * const argv[]);

	/*
	 * Parses one token/flag-argument pair into shared data.
	 *
	 * Takes one step through given tokens, parsing the data into the four
	 * shared data items. Return indicates if parsing resulted in new, useful
	 * data (vs a fully parsed line of tokens).
	 *
	 * @return true if shared data is fresh, false otherwise
	 */
	bool getOpt(void);

private:
	// Internal data saved between parsing calls.
	std::vector<std::string> m_tokens;  // saved cmd line tokens
	std::size_t              m_nextIdx; // next token to parse

	// Helper function to parse asmld.exe specific flags/details.
	void parseFlag(std::string const flagStr);
};

#endif /* DEVICE_GETOPT_H_ */
