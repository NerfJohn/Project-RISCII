/*
 * LexToken_e.h
 *
 * "Building blocks of the assembly language syntax"
 */

#ifndef SRC_DOMAINLAYER_LEXTOKEN_E_H_
#define SRC_DOMAINLAYER_LEXTOKEN_E_H_

/*
 * Definition of "words" of the assembly language.
 *
 * Each lex token is made up of one or more characters. They represent the
 * building blocks of the assembly language- being able to be combined in
 * sequence to form larger constructs.
 */
typedef enum {
	// Core values.
	TOKEN_COMMENT,                   // User comments
	TOKEN_FLAGS,                     // Flag bit(s) variable
	TOKEN_REGISTER,                  // Register file variable
	TOKEN_IMMEDIATE,                 // Constant integer value (decimal or hex)
	TOKEN_STR_LIT,                   // String literal
	TOKEN_EOF,                       // End-of-File character
	TOKEN_LABEL,                     // Label/Address Identifier
	TOKEN_COLON,                     // Colon (used to denote label decls)
	TOKEN_LCURLY,                    // Left Curly brace (for data array)
	TOKEN_RCURLY,                    // Right Cury brace (for data arrray)
	TOKEN__la,                       // Function keyword "_la"
	TOKEN_dbss,                      // keyword ".bss"
	TOKEN_ddata,                     // keyword ".data"
	TOKEN_AND,                       // AND instruction keyword
	TOKEN_ORR,                       // ORR instruction keyword
	TOKEN_XOR,                       // XOR instruction keyword
	TOKEN_SHL,                       // SHL instruction keyword
	TOKEN_SHR,                       // SHR instruction keyword
	TOKEN_ADD,                       // ADD instruction keyword
	TOKEN_SUB,                       // SUB instruction keyword
	TOKEN_LBI,                       // LBI instruction keyword
	TOKEN_LDR,                       // LDR instruction keyword
	TOKEN_STR,                       // STR instruction keyword
	TOKEN_BRC,                       // BRC instruction keyword
	TOKEN_JPR,                       // JPR instruction keyword
	TOKEN_JLR,                       // JLR instruction keyword
	TOKEN_SWP,                       // SWP instruction keyword
	TOKEN_NOP,                       // NOP instruction keyword
	TOKEN_HLT,                       // HLT instruction keyword

	// Special values.
	TOKEN_INVALID,                   // Unknown/Invalid lex token
	TOKEN_LEX_MIN   = TOKEN_COMMENT, // Lowest value lex token
	TOKEN_LEX_MAX   = TOKEN_INVALID  // Highest value lex token
} LexToken_e;

#endif /* SRC_DOMAINLAYER_LEXTOKEN_E_H_ */
