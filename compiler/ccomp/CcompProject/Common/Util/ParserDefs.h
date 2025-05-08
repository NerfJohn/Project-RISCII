/*
 * ParserDefs.h: Macros specialized for usage with Common/Device/Parser.h.
 */

#ifndef COMMON_UTIL_PARSERDEFS_H_
#define COMMON_UTIL_PARSERDEFS_H_

#include <string>
#include <vector>
#include "Common/Domain/RetErr_e.h"

/*
 * @brief Definitions to meet Parser's needs in a streamlined fashion.
 *
 * Parser relies on three user defined functions: parseState(), isTkn(), and
 * isAct(). parseState() breaks down a nonterminal parse token into lesser
 * tokens, while isTkn() and isAct() identifies specific token types (lex token
 * and parse actions respectively).
 *
 * (Note: isTkn() is the same isTkn() from Common/Util/LexerDefs.h.)
 *
 * Each function's signatures are provided below. Copying the signatures ensures
 * the following macros work as intended.
 *
 * Definitions PARSER_<name> are provided as pre-defined states. START is
 * the starting state on the parsing stack. MISMATCH and UNEXPECTED represent
 * unique errors. THIS is a special variable used in parse lists (see below).
 *
 * The LIST() macro is used to define lists of lesser tokens of broken down
 * nonterminal parse tokens. The list is pushed to the parse stack left to
 * right. Items should be lex tokens, parse tokens, and parse actions.
 * PARSER_THIS is replaced with the input token causing the stack push.
 *
 * The IS() and ELSE() macros are used to specify LIST()s to push to the parse
 * stack based on the input token to parseState().
 *
 * It is recommended to make an enum where parse tokens and parse actions are
 * 1) grouped for easy bounds checking and 2) extend the lex tokens in value,
 * such that all three can be present in a list without crossover.
 *
 * For parseState(), it is recommended to create a switch across the nonterminal
 * parse states, using the macros to check/determine the nonterminal's lesser
 * tokens. If no conditions are taken, RET_ERR_ERROR should be returned.
 */

//=== "External Values" ========================================================

// User-specific functions the Parser relies on for parsing.
RetErr_e parseState(int state, int tkn, std::vector<int> const*& seq);
bool isTkn(int state);
bool isAct(int state);

// Definitions used by Parser for user specific values.
#define PARSER_START      -1
#define PARSER_MISMATCH   -2
#define PARSER_UNEXPECTED -3
#define PARSER_THIS       -4

// Macros specialized for creating series of breakdown tokens.
#define LIST(name)  static std::vector<int> const name

// Macros specialized for "parseState" function.
#define TKN_IS(val,lst) if (tkn == (val)) {seq = &(lst); return RET_ERR_NONE;}
#define TKN_ELSE(lst)   seq = &(lst); return RET_ERR_NONE;

#endif /* COMMON_UTIL_PARSERDEFS_H_ */
