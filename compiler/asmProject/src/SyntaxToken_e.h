/*
 * SyntaxToken_e.h
 *
 *  Created on: Jan 10, 2024
 *      Author: John
 */

#ifndef SRC_SYNTAXTOKEN_E_H_
#define SRC_SYNTAXTOKEN_E_H_

// TODO- Tokens found by scanner, used by parser.
typedef enum SyntaxToken_e {
    // Core values.
    TOKEN_AND         = 0,             // Instruction Mnemonics
    TOKEN_ORR         = 1,
    TOKEN_XOR         = 2,
    TOKEN_SHL         = 3,
    TOKEN_SHR         = 4,
    TOKEN_ADD         = 5,
    TOKEN_SUB         = 6,
    TOKEN_LBI         = 7,
    TOKEN_LDR         = 8,
    TOKEN_STR         = 9,
    TOKEN_BRC         = 10,
    TOKEN_JPR         = 11,
    TOKEN_JLR         = 12,
    TOKEN_SWP         = 13,
    TOKEN_NOP         = 14,
    TOKEN_HLT         = 15,            // ~Instruction Mnemonics
    TOKEN_LD          = 16,            // "Load" built-in function
    TOKEN_ST          = 17,            // "Store" built-in function
    TOKEN_LA          = 18,            // "Load Address" built-in function
    TOKEN_TO          = 19,            // "Goto" built-in function
    TOKEN_WORD        = 20,            // .word Directive
    TOKEN_ARRY        = 21,            // .arry Directive
	TOKEN_GLBL        = 22,
	TOKEN_HEAP        = 23,
    TOKEN_IMMEDIATE   = 24,            // Immediate (decimal/hexadecimal)
    TOKEN_REGISTER    = 25,            // Register
    TOKEN_STRING      = 26,            // String Literal
    TOKEN_FLAG        = 27,            // Flags
    TOKEN_LABEL       = 28,            // Label
    TOKEN_COLON       = 29,            // Colon (for label decls)
    TOKEN_COMMENT     = 30,            // Comment

    // Special values.
    TOKEN_EOF         = 31,            // EOF detected/read in
    TOKEN_INVALID     = 32,            // Invalid/Unknown token
	TOKEN_MAX_VALUE   = TOKEN_INVALID  // Highest occupied index by enum
} SyntaxToken_e;

#endif /* SRC_SYNTAXTOKEN_E_H_ */
