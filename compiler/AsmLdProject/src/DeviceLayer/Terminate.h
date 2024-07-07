/*
 * Terminate.h
 *
 * "Device for terminating the application with a given error code"
 */

#ifndef SRC_DEVICELAYER_TERMINATE_H_
#define SRC_DEVICELAYER_TERMINATE_H_

#include "DomainLayer/ExitReasonType_e.h"

/*
 * Device to handle terminating the assembler program when finished.
 *
 * Singleton for handling termination of the program. Ensures return code is
 * provided to the program and passed to its caller to provide information on
 * why the program terminated.
 */
class Terminate {
public:
	/*
	 * Getter for singleton instance. Instantiates on first call.
	 *
	 * @return singleton instance of Terminate
	 */
	static Terminate* getInst(void);

	// Do not allow other forms of construction.
	Terminate(Terminate const&)       = delete;
	void operator= (Terminate const&) = delete;

	/*
	 * Terminates the program with given reason. Does NOT return.
	 *
	 * Has OS terminate program, returning the given exit code to the program's
	 * caller. Due to termination nature, function does not return.
	 *
	 * @param exitCode value to return to program's caller
	 */
	void exit(ExitReasonType_e exitCode);

private:
	// Limit construction/destruction to internal scope.
	Terminate()  {}
	~Terminate() {}
};

#endif /* SRC_DEVICELAYER_TERMINATE_H_ */
