/*
 * LabelTable.h
 *
 *  Created on: Apr 4, 2024
 *      Author: John
 */

#ifndef SRC_ANALYSIS_LABELTABLE_H_
#define SRC_ANALYSIS_LABELTABLE_H_

#include <string>
#include <unordered_map>
#include "LabelInfo_t.h"

// TODO- tracks label names to pertinent info.
class LabelTable {
public:
	// TODO- instance starts anew.
	LabelTable(void);

	// TODO- log label declaration.
	void addDecl(std::string name, ABuildItem* item);

	// TODO- log use of a label.
	void addUse(std::string name);

	// Std. destructor.
	~LabelTable(void);

private:
	// Table tying names to data.
	std::unordered_map<std::string, LabelInfo_t> m_table;
};

#endif /* SRC_ANALYSIS_LABELTABLE_H_ */
