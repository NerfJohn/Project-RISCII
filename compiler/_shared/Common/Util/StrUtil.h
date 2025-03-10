/*
 * StrUtil.h: Utilities for querying/manipulating std strings.
 */

#ifndef COMMON_UTIL_STRUTIL_H_
#define COMMON_UTIL_STRUTIL_H_

#include <cstdint>
#include <string>

/*
 * @brief Removes whitespace from "sides" of string. Ignore "middle" whitespace.
 *
 * Typical 'trim' function for strings- removing commonly ignored whitespace
 * characters from the side of a central string element. Keeps whitespace
 * characters between non-whitespace characters.
 *
 * @param str string to trim whitespace from
 */
void StrUtil_trim(std::string& str);

/*
 * @brief Removes all instances of character in string.
 *
 * @param str string to modify/remove char from
 * @param chr character to remove all instances of
 */
void StrUtil_rmAll(std::string& str, uint8_t chr);

// TODO
void StrUtil_rmFtype(std::string& str);

/*
 * @brief Extract directory from file path.
 *
 * Assumes directories always end with '/' (ie stupid simple approach).
 *
 * @param str string to modify into just a directory
 */
void StrUtil_asDir(std::string& str);

#endif /* COMMON_UTIL_STRUTIL_H_ */
