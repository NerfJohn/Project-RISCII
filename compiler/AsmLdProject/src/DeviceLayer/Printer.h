/*
 * Printer.h
 *
 * "Device handling printing of log messages to stdout"
 */

#ifndef SRC_DEVICELAYER_PRINTER_H_
#define SRC_DEVICELAYER_PRINTER_H_

#include <string>
#include <queue>
#include "DomainLayer/DataModel_t.h"
#include "DomainLayer/LogType_e.h"

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
	Printer(Printer const&) = delete;
	void operator=(Printer const&) = delete;

	/*
	 * Logs (but does not output) basic message.
	 *
	 * Formats and saves message. Message is printed once Printer::printLog is
	 * called.
	 *
	 * @param type grouping message falls under
	 * @param msg message to log for future printing
	 */
	void log(LogType_e type, std::string msg);

	/*
	 * Logs (but does not output) message with related filename.
	 *
	 * Formats and saves message. Message is printed once Printer::printLog is
	 * called.
	 *
	 * @param type grouping message falls under
	 * @param file filename to include in message header
	 * @param msg message to log for future printing
	 */
	void log(LogType_e type, std::string file, std::string msg);

	/*
	 * Logs (but does not output) message with related filename/line number.
	 *
	 * Formats and saves message. Message is printed once Printer::printLog is
	 * called.
	 *
	 * @param type grouping message falls under
	 * @param file filename to include in message header
	 * @param msg message to log for future printing
	 */
	void log(LogType_e type, std::string file, uint32_t line, std::string msg);

	/*
	 * Prints all logged messages (in the order of being logged).
	 *
	 * Function empties the entire saved log. Calling before logging (more)
	 * messages has no effect.
	 *
	 */
	void printLog(void);

	/*
	 * Prints special assert message immediately. Empties log first.
	 *
	 * Calling implictly calls Printer::printLog before printing the assert
	 * message. Caller is responsible for terminating just after printing.
	 *
	 * @param msg custom message for assert message
	 */
	void printAssert(std::string msg);

	/*
	 * Logs a summary of the assembly process as an info message.
	 *
	 * Logs a messages noting the number of warnings/errors found and the status
	 * of the image's ability to be created. Intended to be called just before
	 * terminating (caller must terminate).
	 *
	 * @param model data model of the program
	 */
	void printSummary(DataModel_t const& model);

private:
	// Limit construction/destruction to internal scope.
	Printer()  {}
	~Printer() {}

	// Keep track of logged messages.
	static std::queue<std::string> m_log;
};

#endif /* SRC_DEVICELAYER_PRINTER_H_ */
