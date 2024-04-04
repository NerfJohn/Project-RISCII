/*
 * OperandUtils.h
 *
 *  Created on: Apr 1, 2024
 *      Author: John
 */

#ifndef SRC_DSITEMS_OPERANDUTILS_H_
#define SRC_DSITEMS_OPERANDUTILS_H_

#include <string>
#include <vector>
#include "SectionType_e.h"

// TODO- Converts immediate string (decimal or hex) to int for ease.
int OperandUtils_immStrToInt(std::string immStr);

// TODO- Converts register string to int for ease.
int OperandUtils_regStrToInt(std::string regStr);

// TODO- Converts string literal to vector of bytes.
std::vector<uint8_t> OperandUtils_strLitToBytes(std::string strLit);

// TODO- Converts section to enum for ease.
SectionType_e OperandUtils_sectStrToEnum(std::string sectStr);

#endif /* SRC_DSITEMS_OPERANDUTILS_H_ */
