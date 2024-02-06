/*
 * BogeNodes.h
 *
 *  Created on: Feb 4, 2024
 *      Author: John
 */

#ifndef SRC_BOGENODES_H_
#define SRC_BOGENODES_H_

#include <stack>
#include <stdint.h>
#include <string>
#include "ParseAction_e.h"

// TODO- desc.
typedef enum {
	ITEM_DATA,
	ITEM_INSTR,
	ITEM_BUILT_IN,
	ITEM_LABEL_DECL,
	ITEM_ADDR_DECL,
} ItemType_e;

// TODO- desc.
class IBuildItem {
public:
	// TODO- desc each.
	virtual ItemType_e asItemType(void) = 0;
	virtual ParseAction_e asParseType(void) = 0;
	virtual uint32_t getBinSize(void) = 0;
	virtual std::string getStrInfo(void) = 0;
	virtual uint32_t getIntInfo(void) = 0;

	// TODO- desc.
	virtual ~IBuildItem() {/* Empty dtor */}

protected:
	// TODO- desc each.
	ItemType_e m_itemType;
	ParseAction_e m_parseType;
	uint32_t m_binSize;
};

// TODO- desc.
class InstrItem: public IBuildItem {
public:
	// TODO- desc.
	InstrItem(ParseAction_e parseType, std::stack<SyntaxToken_e>* actStack);
};

// TODO- desc.
class DataItem: public IBuildItem {
public:
	// TODO- desc.
	DataItem(ParseAction_e parseType, std::stack<SyntaxToken_e>* actStack);
};

// TODO- desc.
class LabelItem: public IBuildItem {
public:
	// TODO- desc.
	LabelItem(ParseAction_e parseType, std::stack<SyntaxToken_e>* actStack);
};

// TODO- desc.
class AddrItem: public IBuildItem {
public:
	// TODO- desc.
	AddrItem(ParseAction_e parseType, std::stack<SyntaxToken_e>* actStack);
};

// TODO- desc.
class LabelDeclItem: public IBuildItem {
public:
	// TODO- desc.
	LabelDeclItem(ParseAction_e parseType, std::stack<SyntaxToken_e>* actStack);
};

/*
 * Semantic Analysis: 1-pass (backwards), analyze labels, data/text blocks, and sizes.
 *
 * Analyzed Data:
 * LabelTable- logUse(labelName), logDecl(labelName)
 * DataTable-  logBss(ptr_to_item), logGlobal(ptr_to_item), getBssSize(), getGlobalSize()
 * TextTable-  logAddrBlock(ptr_to_item(s), addr, size), logGeneralBlock(ptr_to_item(s), size), getTextSize()
 * ---
 * locally: curTextSize, lastAddrItem, lastLabelItem
 *
 * for each item in backwards build item list:
 *   if DataItem:
 *   	if curItem.getIntInfo() == 0: logBss(curItem)         // ie if not initialized
 *   	else                        : logGlobal(curItem)
 *   	always                      : lastLabelItem = curItem
 *
 *   if InstrItem:
 *   	curTextSize   = curItem.getBinSize()
 *   	lastLabelItem = curItem
 *   	lastAddrItem  = curItem
 *   	if curItem.getStrInfo() != null:                      // ie if contains label
 *   		logUse(curItem.getStrInfo())
 *
 *   if LabelDeclItem:
 *   	logDecl(curItem.getStrInfo())
 *
 *   if AddrItem and curTextSize != 0:
 *   	if AddrItem.getIntInfo() != -1: logAddrBlock(curItem, getIntInfo(), curTextSize)
 *   	else                          : logGeneralBlock(curItem, curTextSize)
 *   	always                        : curTextSize = 0
 *
 * (end)
 * (last check/handling of text block started by start of first file)
 */

#endif /* SRC_BOGENODES_H_ */
