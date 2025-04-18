/*
 * Msg.h: Utility class for string-ifying log messages.
 */

#ifndef COMMON_UTIL_MSG_H_
#define COMMON_UTIL_MSG_H_

#include <string>

/*
 * @brief Class designed to ease log message creation.
 *
 * Handles specific concatenation requirements of various types. Intended to
 * be immediately assigned to a string. Handles strings, integers, and bools.
 *
 * Contained solely in header to match its sister class "Common/Util/Ptr.h".
 */
class Msg {
public:
	/*
	 * @brief Constructor- naturally empty to start string building.
	 *
	 * @return Msg type ready to be concatenated to
	 */
	Msg(void) {m_str = "";}

	/*
	 * @brief Concatenation overloads. Supports strings/ints/bools.
	 *
	 * Each instance handles the specific requirements needed to string-ify and
	 * concatenate. Designed to be repeatable in a chain.
	 *
	 * @param s string or string literal type
	 * @param i integer- unsigned or signed
	 * @param b boolean- string-ified as "true" or "false"
	 * @return  reference to resulting Msg type
	 */
	Msg& operator+(std::string const& s) {m_str += s; return *this;}
	Msg& operator+(char        const* s) {m_str += s; return *this;}
	Msg& operator+(int      const i) {m_str += std::to_string(i); return *this;}
	Msg& operator+(unsigned const i) {m_str += std::to_string(i); return *this;}
	Msg& operator+(bool b) {m_str += (b) ? "true" : "false"; return *this;}

	/*
	 * @brief Conversion overload. Allows Msg to be assigned to a string.
	 *
	 * @return resulting string from constructed message
	 */
	operator std::string&(void) {return m_str;}

	/*
	 * @brief Std destructor.
	 */
	~Msg(void) {}

private:
	// String being crafted under the hood.
	std::string m_str = "";
};

#endif /* COMMON_UTIL_MSG_H_ */
