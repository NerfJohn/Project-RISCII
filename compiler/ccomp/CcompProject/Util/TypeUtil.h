/*
 * TypeUtil.h: Utilities related to managing type instances.
 */

#ifndef UTIL_TYPEUTIL_H_
#define UTIL_TYPEUTIL_H_

#include <deque>
#include "Common/Ds/LexToken.h"
#include "Common/Util/Ptr.h"
#include "Domain/Type_t.h"

/*
 * @brief Initializes type using series of tokens.
 *
 * Parses tokens into binary type. Intended to both populate and ease type
 * operations. Requires a "core" token be provided.
 *
 * @param type type to initialize
 * @param tkns series of tokens to parse into type
 */
void TypeUtil_initCore(Type_t& type, std::deque<Ptr<LexToken>>& tkns);

// TODO
bool TypeUtil_cmpType(Type_t const& typeA, Type_t const& typeB);

#endif /* UTIL_TYPEUTIL_H_ */
