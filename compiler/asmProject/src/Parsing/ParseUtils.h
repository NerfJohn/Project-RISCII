/*
 * ParseUtils.h
 *
 *  Created on: Apr 1, 2024
 *      Author: John
 */

#ifndef SRC_PARSING_PARSEUTILS_H_
#define SRC_PARSING_PARSEUTILS_H_

#include <string>
#include <vector>
#include <queue>
#include "ABuildItem.h"

#include "../Parsing/AsmToken.h"

// TODO- Lex file into vector of assembler defined tokens. Can fatal error.
std::queue<AsmToken*> ParseUtils_lexFile(std::string fName);

// TODO- Parses token vector into data structure for analysis. Can fatal error.
std::vector<ABuildItem*> ParseUtils_parseTokens(std::queue<AsmToken*> tokens);

#endif /* SRC_PARSING_PARSEUTILS_H_ */
