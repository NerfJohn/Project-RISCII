/*
 * GetOpt.h: Customized version of unistd.h's "getopt" function.
 */

#ifndef COMMON_DEVICE_GETOPT_H_
#define COMMON_DEVICE_GETOPT_H_

#include <string>
#include <vector>
#include "Domain/CliType_e.h"

/*
 * @brief Specialized "parser" for cli arguments- steps through given arguments.
 * 
 * Custom version of "getopt" function- capable of parsing files and flags (for
 * given flag prefix and 0-1 separated arguments per flag). Also identifies
 * command parsing errors. Designed for user to iteratively handling findings.
 */
class GetOpt {
public:
	// Data parsed per iteration- shared with user.
	CliType_e   m_type;   // result of last iteration
	std::string m_value;  // raw value of parsed token
	std::string m_arg;    // argument of token (as applicable, else empty)

	/*
	 * @brief Constructor- preps to parse given arguments (with flag finder).
	 * 
	 * Parses argc/argv into list of cli arguments to parse (ignoring null/empty
	 * strings). Uses given function to convert strings into flags (denoted by
	 * their corresponding CliType_e ID).
	 * 
	 * @param argc   number of tokens in argv
	 * @param argv   array of string tokens to parse
	 * @param asFlag ptr to "string to type" function for flags
	 */
	GetOpt(int const          argc, 
		   char const * const argv[],
		   CliType_e         (asFlag*)(std::string));	
	
	/*
	 * @brief Parses one token/flag-argument pair into shared data.
	 * 
	 * Takes one "step" through given tokens, parsing findings into shared data.
	 * Return indicates if step was carried out (vs being at the end and can't
	 * step forward).
	 * 
	 * @return true if step was taken/data was updated, false otherwise
	 */
	bool getOpt(void);

private:
	// Internal data saved between parsing calls.
	std::vector<std::string> m_tokens;  // saved cmd line tokens
	std::size_t              m_nextIdx; // next token to parse
	
	// Saved ptr to "string to flag type" function.
	CliType_e (m_asFlag*)(std::string);
};

#endif /* COMMON_DEVICE_GETOPT_H_ */
