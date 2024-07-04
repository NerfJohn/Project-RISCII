/*
 * Printer.h
 *
 * "Device handling printing of log messages to stdout"
 */

#ifndef SRC_DEVICELAYER_PRINTER_H_
#define SRC_DEVICELAYER_PRINTER_H_

#include <string>
#include "DomainLayer/DataModel_t.h"
#include "DomainLayer/LogType.h"

/*
 * Device for printing messages according to formatting/business logic.
 *
 * Singleton class that abstracts program/type formatting from caller, but
 * relies on caller to create custom messages and locations.
 */
class Printer {
public:
	/*
	 * Getter for singleton instance. Instantiates on first call.
	 *
	 * @return singleton instance of Printer
	 */
	static Printer* getInst(void);

	// Do not allow other forms of construction.
	Printer(Printer const&)         = delete;
	void operator= (Printer const&) = delete;

	/*
	 * Print a given message to stdout. Type specifies message grouping.
	 *
	 * Formats message to unify stdout output from program. Always appends a
	 * newline to the custom message.
	 *
	 * @param type type of message with respect to logging
	 * @param msg custom message to display
	 */
	void log(LogType_e type, std::string msg);

	/*
	 * Print a given message to stdout. Includes given location in message.
	 *
	 * Formats message to unify stdout output from program. Always appends a
	 * newline to the custom message.
	 *
	 * @param type type of message with respect to logging
	 * @param location file and line to report with message
	 * @param msg custom message to display
	 */
	void log(LogType_e type, std::string location, std::string msg);

private:
	// Limit construction/destruction to internal scope.
	Printer()  {}
	~Printer() {}
};

#endif /* SRC_DEVICELAYER_PRINTER_H_ */
