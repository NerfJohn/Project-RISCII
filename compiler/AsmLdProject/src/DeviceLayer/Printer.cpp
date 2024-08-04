/*
 * Printer.cpp
 *
 * "Device handling printing of log messages to stdout"
 */

#include "DeviceLayer/Terminate.h"
#include "OsLayer/OsStdout.h"
#include "Utils/PrintUtils.h"

#include "DeviceLayer/Printer.h"

using namespace std;

//==============================================================================

// Development "switch" for block/non-blocking printing (eases deep debugging).
//#define DEV_PRINT_ON_LOG

//==============================================================================

// Log level threshold for printing.
LogType_e Printer::m_threshLevel = LOG_WARN;

// Logged messages awaiting printing.
std::queue<std::string> Printer::m_log = queue<string>();

//==============================================================================
// Getter for singleton instance. Instantiates on first call.
Printer* Printer::getInst(void) {
	// Static instance- implicitly creates one (and only one) instance.
	static Printer instance;

	// Return pointer to instance.
	return &instance;
}

//==============================================================================
// Sets the log level used to filter which messages may be printed.
void Printer::setLogLevel(LogType_e level) {
	m_threshLevel = level;
}

//==============================================================================
// Logs (but does not output) basic message.
void Printer::log(LogType_e type, std::string msg) {
	// Begin creating message to log.
	string logStr = msg;

	// Format into stdout message- ensuring formatted goes well.
	if (!PrintUtils_formatLog(type, &logStr)) {
		// Unexpected bad formatting (likely design bug)- assert!
		this->printAssert("unable to format basic message properly");
		Terminate::getInst()->exit(REASON_ASSERT);
	}

	// Message formatted- save to log (if applicable).
	if (type <= m_threshLevel) {m_log.push(logStr);}

#ifdef DEV_PRINT_ON_LOG
	// Immediately print logged information.
	this->printLog();
#endif // DEV_PRINT_ON_LOG
}

//==============================================================================
// Logs (but does not output) message with related filename.
void Printer::log(LogType_e type, std::string file, std::string msg) {
	// Begin creating message to log.
	string logStr = msg;

	// Format into stdout message- ensuring formatted goes well.
	if (!PrintUtils_formatLog(type, file, &logStr)) {
		// Unexpected bad formatting (likely design bug)- assert!
		this->printAssert("unable to format file message properly");
		Terminate::getInst()->exit(REASON_ASSERT);
	}

	// Message formatted- save to log (if applicable).
	if (type <= m_threshLevel) {m_log.push(logStr);}

#ifdef DEV_PRINT_ON_LOG
	// Immediately print logged information.
	this->printLog();
#endif // DEV_PRINT_ON_LOG
}

//==============================================================================
// Logs (but does not output) message with related filename/line number.
void Printer::log(LogType_e type,
		          std::string file,
				  uint32_t line,
				  std::string msg
				 ) {
	// Begin creating message to log.
	string logStr = msg;

	// Format into stdout message- ensuring formatted goes well.
	if (!PrintUtils_formatLog(type, file, line, &logStr)) {
		// Unexpected bad formatting (likely design bug)- assert!
		this->printAssert("unable to format file/line message properly");
		Terminate::getInst()->exit(REASON_ASSERT);
	}

	// Message formatted- save to log (if applicable).
	if (type <= m_threshLevel) {m_log.push(logStr);}

#ifdef DEV_PRINT_ON_LOG
	// Immediately print logged information.
	this->printLog();
#endif // DEV_PRINT_ON_LOG
}

//==============================================================================
// Prints all logged messages (in the order of being logged).
void Printer::printLog(void) {
	// Empty log, printing each message in order.
	while (!m_log.empty()) {
		// Print message to stdout.
		OsStdout_printStringLn(m_log.front());

		// Flush the message from the log.
		m_log.pop();
	}
}

//==============================================================================
// Prints special assert message immediately. Empties log first.
void Printer::printAssert(std::string msg) {
	// Begin creating message to log.
	string assertStr = msg;

	// Re-format to become an assert.
	PrintUtils_formatAssert(&assertStr);

	// Empty log for info about events leading to assert (ended with assert).
	this->printLog();
	OsStdout_printStringLn(assertStr);
}

//==============================================================================
// Logs a summary of the assembly process as an info message.
void Printer::printSummary(DataModel_t const& model) {
	// String to log.
	string logStr;

	// Format summary message.
	int retErr = PrintUtils_formatSummary(model, logStr);
	if (retErr) {
		// Should be a safe operation- design flaw!
		this->printAssert("unable to format summary properly");
		Terminate::getInst()->exit(REASON_ASSERT);
	}

	// Message formatted- save to log (if applicable).
	if (LOG_INFO <= m_threshLevel) {m_log.push(logStr);}

#ifdef DEV_PRINT_ON_LOG
	// Immediately print logged information.
	this->printLog();
#endif // DEV_PRINT_ON_LOG
}

//==============================================================================
// Prints special help menu message immediately. Does NOT empty log.
void Printer::printHelp(void) {
	// Get help menu string.
	string msg = PrintUtils_formatHelp();

	// Aaaand print it- pretty simple.
	OsStdout_printStringLn(msg);
}
