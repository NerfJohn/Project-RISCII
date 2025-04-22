/*
 * GetOpt.h: Customized version of unistd.h's "getopt" function.
 */

#ifndef COMMON_DEVICE_GETOPT_H_
#define COMMON_DEVICE_GETOPT_H_

#include <string>
#include <vector>
#include "Common/Domain/CliType_e.h"

/*
 * @brief Customized "getopt" function for grabbing files/flags.
 *
 * Iterates over input- categorizing tokens as 1) file 2) 'lone' flag and 3)
 * flag with 1 non-flag argument just after flag. User defines function to
 * identify flags. Results shared via public variables.
 */
class GetOpt {
public:
	// Parsed data- shared with user.
	CliType_e   m_type;     // resulting (error?) type
	int         m_flagCode; // user specific result
	std::string m_value;    // raw value of token
	std::string m_arg;      // flag argument (empty if not applicable)

	/*
	 * @brief Constructor- parses cli arguments into iterable list.
	 *
	 * @param argc length of argv (ie number of tokens)
	 * @param argv array of string-ified cli arguments
	 */
	GetOpt(int const argc, char const* const argv[]);

	/*
	 * @brief Iterates one step through arguments- updating public variables.
	 *
	 * Public variables indicate result of cli parsing. Return indicates if any
	 * actual parsing work was done (ie has finished parsing all arguments).
	 *
	 * @return true if cli argument(s) was parsed, false otherwise
	 */
	bool get(void);

private:
	// Internal data saved between parsing calls.
	std::vector<std::string> m_tokens;  // saved cmd line tokens
	std::size_t              m_nextIdx; // next token to parse
};

#endif /* COMMON_DEVICE_GETOPT_H_ */
