/*
 * ParserDefs.h: Macros specialized for usage with Common/Device/Parser.h.
 */

#ifndef COMMON_UTIL_PARSERDEFS_H_
#define COMMON_UTIL_PARSERDEFS_H_

#include <string>

// Macros for creating sequences (ie to return from parsing state).
#define LIST(name)            static std::vector<int> const name

// Macros for 'parse' transitions (ie "match condition? return sequence").
#define WITH(tkn,cmp,ptr,seq) if ((tkn) == (cmp))                         \
                              {*(ptr) = &(seq); return PARSE_OP_INCLUDE;}
#define ONLY(tkn,cmp,ptr,seq) if ((tkn) == (cmp))                         \
                              {*(ptr) = &(seq); return PARSE_OP_EXCLUDE;}
#define EONL(ptr,seq)         *(ptr) = &(seq); return PARSE_OP_EXCLUDE;

#endif /* COMMON_UTIL_PARSERDEFS_H_ */
