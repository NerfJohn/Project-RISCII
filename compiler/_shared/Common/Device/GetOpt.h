/*
 * GetOpt.h: Customized version of unistd.h's "getopt" function.
 */

#ifndef COMMON_DEVICE_GETOPT_H_
#define COMMON_DEVICE_GETOPT_H_

#include <vector>
#include "Common/Domain/CliType_e.h"
#include "Common/Domain/OptInit_t.h"

/*
 * @brief Customized GetOpt capable of parsing files/flags/arguments.
 *
 * Enhanced version of "getopt" function (tailored for RISCII compiler use).
 * Instances can parse through command line input to extract file and flag info
 * in an iterable manner. Requires callbacks/user handled scenarios.
 */
class GetOpt {
public:
	// Parsed data shared with the user.
	CliType_e   m_type;     // type of token parsed
	int         m_flagCode; // 'flag enum' specifying parsed flag value
	std::string m_value;    // raw value of token
	std::string m_arg;      // argument of flag (as applicable, empty if none)

	/*
	 * @brief Constructor- inits cli parsing for given input data.
	 *
	 * Saves local copy of cli arguments (ignoring null/empty strings). Also
	 * saves pointers to callback handlers- requiring neither be null.
	 *
	 * @param initData data used to prepare parsing of cli arguments
	 */
	GetOpt(OptInit_t const& initData);

	/*
	 * @brief Parses one token/flag-argument pair into shared data.
	 *
	 * Takes one step through given tokens, parsing findings into shared data.
	 * Return indicates if shared data is new. Function error checks, but relies
	 * on caller to handle printing/termination.
	 *
	 * @return true if shared data is fresh, false otherwise
	 */
	bool getOpt(void);

private:
	// Internal data saved between parsing calls.
	std::vector<std::string> m_tokens;  // saved cmd line tokens
	std::size_t              m_nextIdx; // next token to parse

	// Callback functions/handlers defined by user.
	int  (*m_asFlag)(std::string);
	bool (*m_needArg)(int flagCode);

	// Special 'flag enum' values.
	int m_badFlag;
};

#endif /* COMMON_DEVICE_GETOPT_H_ */
