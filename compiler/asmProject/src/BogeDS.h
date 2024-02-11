/*
 * BogeDS.h
 *
 *  Created on: Feb 6, 2024
 *      Author: John
 */

#ifndef SRC_BOGEDS_H_
#define SRC_BOGEDS_H_

#include <iostream>
#include <string>
#include <vector>
#include "BogeNodes.h"

// TODO- desc.
typedef struct {
	std::string m_name;
	bool isDeclared;
	bool isUsed;
} LabelInfo_t;

// TODO- desc.
typedef struct {
	IBuildItem* m_item;
	int m_size;
} TextBlock_t;

// TODO- desc.
class LabelTable {
public:
	// TODO- desc.
	bool logDecl(std::string labelName);
	void logUse(std::string labelName);

	// TODO- desc.
	void print(void);

private:
	// TODO- desc.
	std::vector<LabelInfo_t> m_labels;
};

// TODO- desc.
class DataTable {
public:
	// TODO- desc.
	void logBss(IBuildItem* item);
	void logGlobal(IBuildItem* item);

	// TODO- desc.
	void print(void);

private:
	// TODO- desc.
	std::vector<IBuildItem*> m_bssItems;
	std::vector<IBuildItem*> m_globalItems;

	// TODO- desc.
	int m_bssSize;
	int m_globalSize;
};

// TODO- desc.
class TextTable {
public:
	// TODO- desc.
	void logMainBlock(IBuildItem* item, int size);
	void logOtherBlock(IBuildItem* item, int size);

	// TODO- desc.
	void print(void);

private:
	// TODO- desc.
	TextBlock_t m_mainBlock;
	TextBlock_t m_otherBlock;
};

#endif /* SRC_BOGEDS_H_ */
