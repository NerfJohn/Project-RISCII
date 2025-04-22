/*
 * LexerDefs.h: Macros specialized for usage with Common/Device/Lexer.h.
 */

#ifndef COMMON_UTIL_LEXERDEFS_H_
#define COMMON_UTIL_LEXERDEFS_H_

#include <cstdint>
#include <string>

/*
 * @brief Definitions to meet Lexer's needs in a streamlined fashion.
 *
 * Lexer relies on two user defined functions: nextState() and isTkn().
 * nextState() manages state transitions using user defined ruleset while
 * isTkn() identifies which states are terminals/tokens.
 *
 * Each function's signatures are provided below. Copying the signatures ensures
 * the following macros work as intended.
 *
 * Definitions LEXER_START and LEXER_ERROR are provided as pre-defined states.
 * LEXER_START is the starting state of all lex paths and LEX_ERROR is the
 * ending state for undefined lex paths.
 *
 * The following macros are intended to simplify writing nextState(). Each will
 * automatically check the input chr (or buffer) and return the next state as
 * indicated. In all cases, 'out' is the state it will automatically return.
 *
 *     IS(lit,out)     checks "chr == lit"
 *     IN(min,max,out) checks "min <= chr <= max"
 *     LABEL(out)      checks "chr" is in {0-9,a-z,A-Z} or an underscore
 *     HEX(out)        checks "chr" is in {0-9,a-f,A-F}
 *     WS(out)         checks "chr" is {\n\r\t} or a space character
 *     EOFILE(out)     checks "chr == 0xFF" (ie EOF character)
 *     ELSE(out)       returns "out" no matter what
 *     ISWORD(str,out) checks "str == buffer"
 *
 * It is recommended to make an enum of all lex states (terminal and not). This
 * enum should set it's starting and error values to LEXER_START and LEXER_ERROR
 * respectively. Grouping terminal tokens can make isTkn() a bounds check.
 *
 * For nextState(), it is recommended to create a switch across the nonterminal
 * lex states. Within each switch, use the macros to guide state transitions.
 * Additional logic to process keywords after the switch may be preferred.
 */

//=== "Internal Values" ========================================================

// ("Internal" definitions for creating more useful ones.)
#define _IS(in,chr)     ((in) == (chr))
#define _IN(in,min,max) (((min) <= (in)) && ((in) <= (max)))

//=== "External Values" ========================================================

// User-specific functions Lexer relies on for lexing.
int nextState(int state, std::string const& buffer, uint8_t chr);
bool isTkn(int state);

// Definitions used by Lexer for user specific values.
#define LEXER_START -1
#define LEXER_ERROR -2

// Macros specialized for "nextState" function.
#define IS(lit,out)     if (_IS(chr,lit)) {return(out); break;}
#define IN(min,max,out) if (_IN(chr,min,max)) {return (out); break;}
#define LABEL(out)      if ((_IN(chr,'a','z')) || (_IN(chr,'A','Z')) || \
		                    (_IN(chr,'0','9')) || (_IS(chr,'_')))       \
						   {return (out); break;}
#define HEX(out)        if ((_IN(chr,'a','f')) || (_IN(chr,'A','F')) || \
                            (_IN(chr,'0','9')))                         \
	                       {return (out); break;}
#define WS(out)         if ((_IS(chr,' '))  || (_IS(chr,'\n'))       || \
                            (_IS(chr,'\r')) || (_IS(chr,'\t')))         \
	                       {return (out); break;}
#define EOFILE(out)     if (_IS(chr,0xFF)) {return (out); break;}
#define ELSE(out)       return (out); break;
#define ISWORD(str,out) if (buffer.compare(str) == 0) {return (out);}

#endif /* COMMON_UTIL_LEXERDEFS_H_ */
