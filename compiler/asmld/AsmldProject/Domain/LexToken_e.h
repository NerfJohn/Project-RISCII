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
	TOKEN_STRLIT,                    // string literal
	TOKEN_COLON,                     // ':' character
	TOKEN_LCURLY,                    // '{' character
	TOKEN_RCURLY,                    // '}' character
	TOKEN_KW_DATA,                   // ".data"       keyword
	TOKEN_KW_BSS,                    // ".bss"        keyword
	TOKEN_KW_GLOBAL,                 // ".global"     keyword
	TOKEN_KW_WEAK,                   // ".weak"       keyword
	TOKEN_KW_LA,                     // "_la"         keyword
	TOKEN_KW_AND,                    // AND instruction keyword
	TOKEN_KW_ORR,                    // ORR instruction keyword
	TOKEN_KW_XOR,                    // XOR instruction keyword
	TOKEN_KW_SHL,                    // SHL instruction keyword
	TOKEN_KW_SHR,                    // SHR instruction keyword
	TOKEN_KW_ADD,                    // ADD instruction keyword
	TOKEN_KW_SUB,                    // SUB instruction keyword
	TOKEN_KW_LBI,                    // LBI instruction keyword
	TOKEN_KW_LDR,                    // LDR instruction keyword
	TOKEN_KW_STR,                    // STR instruction keyword
	TOKEN_KW_BRC,                    // BRC instruction keyword
	TOKEN_KW_JPR,                    // JPR instruction keyword
	TOKEN_KW_JLR,                    // JLR instruction keyword
	TOKEN_KW_SWP,                    // SWP instruction keyword
	TOKEN_KW_NOP,                    // NOP instruction keyword
	TOKEN_KW_HLT,                    // HLT instruction keyword

	// Special values.
	TOKEN_INVALID,                   // Unknown/Invalid lex token
	TOKEN_LEX_MIN   = TOKEN_COMMENT, // Lowest value lex token
	TOKEN_LEX_MAX   = TOKEN_INVALID  // Highest value lex token
} LexToken_e;

#endif /* DOMAIN_LEXTOKEN_E_H_ */
