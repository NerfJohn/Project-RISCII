/*
 * StringUtils.h
 *
 * "Helper functions for handling strings"
 */

#ifndef SRC_UTILS_STRINGUTILS_H_
#define SRC_UTILS_STRINGUTILS_H_

#include <string>
#include <vector>
#include "DomainLayer/RetErr_e.h"

/*
 * Generates list of repeated chars in given string.
 *
 * Iterates through string and returns a list of chars that were repeated at
 * least once. Chars repeated multiple times (i.e. 3+ instances) will show
 * up only once in the returned vector.
 *
 * @pararm inText string to search for duplicate chars in
 * @return vector of chars that were repeated
 */
std::vector<uint8_t> StringUtils_getRepeatChars(std::string inText);

/*
 * Removes duplicate chars from a given string. Retains first instances/order.
 *
 * Iterates through string and eliminates repeated chars. The first instance of
 * each char is kept- with 2nd/3rd/etc instance being deleted from the returned
 * string.
 *
 * @pararm inText string to search for duplicate chars in
 * @return string with same char as input, but without repeated chars
 */
std::string StringUtils_getNoRepeats(std::string inText);

/*
 * Creates list of chars used for unrecognized escapes in given string.
 *
 * Intended to find unrecognized escape sequences in parsed string literals.
 * Unresolved escapes (ie backslash at end of string) cannot be indicated
 * directly, but will result in a failed return code.
 *
 * @param strLit string literal to scan for bad escapes
 * @param chars created list of found bad escapes
 * @return zero for success, non-zero otherwise
 */
RetErr_e StringUtils_getBadEscapes(std::string strLit, std::string& chars);

/*
 * Converts string into bytes. Handles escape chars and ignores wrapping quotes.
 *
 * Intended to get true byte values of a string literal. Escape chars are
 * converted into one character, and wrapping quotes are ignored for
 * convenience. Bytes are in same order as they appear in string literal.
 *
 * @param strLit string literal to convert to bytes
 * @param bytes list of bytes created from string literal
 * @return zero for success, non-zero otherwise
 */
RetErr_e StringUtils_asBytes(std::string const strLit,
		                     std::vector<uint8_t>& bytes);

/*
 * Converts string-ified uint into a typical uint. Handles decimal and hex.
 *
 * Uses given string to set pointed to uint. Return value indicates if the
 * conversion was successful. Can parse both decimal and hexadecimal values.
 *
 * @param uintStr string resembling the uint to extract
 * @param uint place to put the value post conversion
 * @return true if conversion was successful, false otherwise
 */
bool StringUtils_asUint(std::string uintStr, uint32_t* uint);

/*
 * Converts string-ified int into a typical int. Handles decimal and hex.
 *
 * Uses given string to set pointed to int. Return value indicates if the
 * conversion was successful. Can parse both decimal and hexadecimal values.
 *
 * @param intStr string resembling the int to extract
 * @param intPtr place to put the value post conversion
 * @return true if conversion was successful, false otherwise
 */
bool StringUtils_asInt(std::string intStr, int32_t* intPtr);

/*
 * Removes "type" portion from filename (based on dot operator).
 *
 * Attempts to remove a file type from the given string by finding the dot
 * operator. Separates based on the first dot found. If no dot is found, the
 * returned string is the same as the given string.
 *
 * @param filename filename to remove the file type from
 * @return name of the file (ie filename without file type)
 */
std::string StringUtils_removeFileType(std::string filename);

#endif /* SRC_UTILS_STRINGUTILS_H_ */
