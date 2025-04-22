/*
 * Lexer.h: Iteratively lexes given file.
 */

#ifndef COMMON_DEVICE_LEXER_H_
#define COMMON_DEVICE_LEXER_H_

#include <cstdint>
#include <string>

/*
 * @brief Lexer core to lex file into stream of user defined tokens.
 *
 * Iterates over specified file- grouping chars into user defined tokens. Relies
 * on user defined functions to identify tokens/rule-set. Manages file
 * operations for user.
 */
class Lexer {
public:
	// Parsed data shared with user.
	int         m_tknCode; // resulting (error?) token
	std::string m_value;   // raw value of token
	uint32_t    m_line;    // line number of (start of) token

	/*
	 * @brief Constructor- opens specified file for lexing.
	 *
	 * Result of file operation available via fileOpened(). lex() should NOT
	 * be called if fileOpened() returns false.
	 *
	 * @param fname filename to open for lexing
	 */
	Lexer(std::string const& fname);

	/*
	 * @brief Getter for result of file read operation.
	 *
	 * @return true for success in opening read file, false otherwise
	 */
	bool fileOpened(void);

	/*
	 * @brief Lexes file for one token. Returns for token or lexing error.
	 *
	 * Lexes opened file, starting from LEX_START state. Does not save line
	 * number or characters until LEX_START state is left. Shared data is
	 * updated upon return.
	 */
	void lex(void);

	/*
	 * @brief Destructor- closes file prior to destruction.
	 */
	~Lexer(void);

private:
	// Internal data saved between lexing/checking calls.
	bool     m_fileOpened; // result of file operations
	uint32_t m_curLine;    // current line number (vs start of lex token)
};

#endif /* COMMON_DEVICE_LEXER_H_ */
