/*
 * LexToken_e.h: Types of "building blocks" found within programming language.
 */

#ifndef DOMAIN_LEXTOKEN_E_H_
#define DOMAIN_LEXTOKEN_E_H_

/*
 * Definition of "words" of the programming language.
 *
 * Each lex token is made up of one or more characters. They represent the
 * building blocks of the programming language- being able to be combined in
 * sequence to form larger constructs.
 */
typedef enum {
	// Real values.
	TOKEN_COMMENT,                   // one line user comment
	TOKEN_EOF,                       // "end of file" character
	TOKEN_FLAGS,                     // flag(s) variable
	TOKEN_REGISTER,                  // register variable
	TOKEN_IMMEDIATE,                 // immediate/integer variable
	TOKEN_LABEL,                     // labeled address
	TOKEN_COLON,                     // ':' character
	TOKEN_KW_DATA,                   // ".data"       keyword
	TOKEN_KW_SHR,                    // "shift right" keyword

	// Special values.
	TOKEN_INVALID,                   // Unknown/Invalid lex token
	TOKEN_LEX_MIN   = TOKEN_COMMENT, // Lowest value lex token
	TOKEN_LEX_MAX   = TOKEN_INVALID  // Highest value lex token
} LexToken_e;

#endif /* DOMAIN_LEXTOKEN_E_H_ */
