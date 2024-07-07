/*
 * Parser.h
 *
 * "Device to handle parsing of a stream of scan tokens into build items"
 */

#ifndef SRC_DEVICELAYER_PARSER_H_
#define SRC_DEVICELAYER_PARSER_H_

#include <queue>
#include "DomainLayer/DataModel_t.h"
#include "DomainLayer/ScanToken_t.h"
#include "Items/IBuildItem.h"

/*
 * Device for parsing scan tokens into build items.
 *
 * Singleton class encapsulates "token array" to "item array" translation.
 * Success of translation is inferred by returning item list via pointer.
 * Ownership of scan token data is transferred to build items.
 */
class Parser {
public:
	/*
	 * Getter for singleton instance. Instantiates on first call.
	 *
	 * @return singleton instance of Parser
	 */
	static Parser* getInst(void);

	// Do not allow other forms of construction.
	Parser(Parser const&)           = delete;
	void operator= (Parser const&)  = delete;

	/*
	 * Parses scan tokens into list of build items. Returns nullptr on failure.
	 *
	 * Modifies token list by passing the data to the constructed build items.
	 * Logs it's own errors, but relies on caller to print/terminate. Returned
	 * pointer's ownership is transferred on function return.
	 *
	 * @param model data model of the program
	 * @param tokens list of scan tokens to parse into new build items
	 * @return list of build items on success, otherwise nullptr
	 */
	std::queue<IBuildItem*>* parseTokens(DataModel_t* model,
			                             std::queue<ScanToken_t*>* tokens
										);

private:
	// Limit construction/destruction to internal scope.
	Parser()  {}
	~Parser() {}
};

#endif /* SRC_DEVICELAYER_PARSER_H_ */
