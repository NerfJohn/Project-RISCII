/*
 * LexScanner.h
 *
 * "Device to conduct scanning/lexing of given read file"
 */

#ifndef SRC_DEVICELAYER_LEXSCANNER_H_
#define SRC_DEVICELAYER_LEXSCANNER_H_

#include <queue>
#include "DeviceLayer/FileReader.h"
#include "DomainLayer/DataModel_t.h"
#include "DomainLayer/ScanToken_t.h"

/*
 * Device for scanning/lexing a given read file for tokens.
 *
 * Singleton class encapsulates "char array" to "token array" translation.
 * Success of the translation is inferred by returning of token list via
 * pointer (vs a nullptr for no success).
 */
class LexScanner {
public:
	/*
	 * Getter for singleton instance. Instantiates on first call.
	 *
	 * @return singleton instance of LexScanner
	 */
	static LexScanner* getInst(void);

	// Do not allow other forms of construction.
	LexScanner(LexScanner const&)       = delete;
	void operator= (LexScanner const&)  = delete;

	/*
	 * Scans read file into list of tokens. Returns nullptr on failure.
	 *
	 * Modifies read file in order to lex its contents. Logs its own errors, but
	 * relies on caller to print/terminate as needed. Returned pointers'
	 * ownership becomes caller's upon function return.
	 *
	 * @param model data model of the program
	 * @param file read file to scan/lex for tokens
	 * @return ordered list of tokens on success, nullptr otherwise
	 */
	std::queue<ScanToken_t*>* scanFile(DataModel_t* model, FileReader* file);

private:
	// Limit construction/destruction to internal scope.
	LexScanner()  {}
	~LexScanner() {}
};

#endif /* SRC_DEVICELAYER_LEXSCANNER_H_ */
