/*
 * StrUtil.h: Utilities for querying/manipulating std strings.
 */

#ifndef UTIL_STRUTIL_H_
#define UTIL_STRUTIL_H_

#include <string>

/*
 * Removes whitespace from "sides" of given string. Ignore "middle" whitespace.
 *
 * Typical 'trim' function for strings- removing commonly ignored whitespace
 * characters from the side of a central string element. Keeps whitespace
 * characters between non-whitespace characters.
 *
 * @param str string to trim whitespace from
 */
void StrUtil_trim(std::string& str);

#endif /* UTIL_STRUTIL_H_ */
