/*
 * LabelTable.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: John
 */

#include "LabelTable.h"

using namespace std;

// TODO- instance starts anew.
LabelTable::LabelTable(void) {
	// Init fresh table;
	m_table = {};
}

// TODO- log label declaration.
void LabelTable::addDecl(std::string name, ABuildItem* item) {
	// Determine if update or creation is needed.
	if (m_table.find(name) != m_table.end()) {
		// Label already recorded- update item (TODO- arbitrary. Should do?).
		m_table[name].m_item = item;
	}
	else {
		// Label NOT recorded- create new entry.
		LabelInfo_t newInfo = {
			.m_numUses = 0,
			.m_item = item
		};
		m_table[name] = newInfo;
	}
}

// TODO- log use of a label.
void LabelTable::addUse(std::string name) {
	// Determine if update or creation is needed.
	if (m_table.find(name) != m_table.end()) {
		// Label already recorded- update info.
		m_table[name].m_numUses++;
	}
	else {
		// Label NOT recorded- create new entry.
		LabelInfo_t newInfo = {
			.m_numUses = 1,    // factor for use from call
			.m_item = nullptr  // just created- no tied item
		};
		m_table[name] = newInfo;
	}
}

// Std. destructor.
LabelTable::~LabelTable(void) {/* no non-str pointers */}
