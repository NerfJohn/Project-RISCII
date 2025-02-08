/*
 * Terminate.h: Standardizes program exit scenarios.
 */

#ifndef COMMON_DEVICE_TERMINATE_H_
#define COMMON_DEVICE_TERMINATE_H_

#include <string>
#include "Common/Domain/PrgmInfo_t.h"

// Condensed "assert if null" condition.
#define IF_NULL(ptr,msg) if ((ptr) == nullptr) {Terminate::inst().assert(msg);}

/*
 * @brief Singleton class providing standard termination behaviors.
 *
 * Provides termination various common termination procedures. Requires minimal
 * setup from user in setAssertCode() (ie set user's exit code for assertions).
 * Makes minor use of logging as applicable.
 */
class Terminate {
public:
	// Enforce Terminate class as a singleton.
	Terminate(Terminate const&)          = delete;
	void operator=(Terminate const&)     = delete;

	/*
	 * @brief Retrieves the singleton instance for operations.
	 *
	 * @return reference to Terminate singleton
	 */
	static Terminate& inst(void);

	/*
	 * @brief Sets assert exit code for assert().
	 *
	 * Same code is used for any call of assert(). Can be set multiple times.
	 * Default value is 1.
	 *
	 * @param exitCode return code to use for assert().
	 */
	void setAssertCode(int const exitCode);

	/*
	 * @brief Terminates program (without output besides given return code).
	 *
	 * @param exitCode value to return to caller
	 */
	void silent(int const exitCode);

	/*
	 * @brief Prints given message as assert, then terminates.
	 *
	 * Exits with code provided by setAssertCode().
	 *
	 * @param msg message to print as assertion
	 */
	void assert(std::string const& msg);

	/*
	 * @brief Prints summary of program's status, then terminates.
	 *
	 * Prints (as LOG_INFO) counts for recorded warnings/errors and exit code
	 * value (as stored in given model). Terminates using reported exit code.
	 *
	 * @param model program's summary info
	 */
	void summary(PrgmInfo_t const& model);

private:
	// Local save of exit code settings.
	static int s_assertCode;

	// Hide constructor/destructor to force as singleton.
	Terminate()  {/* no actions */}
	~Terminate() {/* no actions */}
};

#endif /* COMMON_DEVICE_TERMINATE_H_ */
