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
    TOKEN_LD          = 16,            // "Load" Macro
    TOKEN_ST          = 17,            // "Store" Macro
    TOKEN_DS          = 18,            // "Data Stack Address" Macro
    TOKEN_TO          = 19,            // "Goto" Macro
    TOKEN_WORD        = 20,            // .word Directive
    TOKEN_ARRY        = 21,            // .arry Directive
    TOKEN_ADDR        = 22,            // .addr Directive
    TOKEN_IMMEDIATE   = 23,            // Immediate (decimal/hexadecimal)
    TOKEN_REGISTER    = 24,            // Register
    TOKEN_STRING      = 25,            // String Literal
    TOKEN_FLAG        = 26,            // Flags
    TOKEN_LABEL       = 27,            // Label
    TOKEN_COLON       = 28,            // Colon (for label decls)
    TOKEN_COMMENT     = 29,            // Comment

    // Special values.
    TOKEN_EOF         = 30,            // EOF detected/read in
    TOKEN_INVALID     = 31,            // Invalid/Unknown token
    TOKEN_VAL_MIN     = TOKEN_AND,     // Lowest token value
    TOKEN_VAL_MAX     = TOKEN_INVALID, // Highest token value
} SyntaxToken_e;

#endif /* SRC_SYNTAXTOKEN_E_H_ */
