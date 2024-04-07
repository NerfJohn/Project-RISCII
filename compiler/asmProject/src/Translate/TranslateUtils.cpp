/*
 * TranslateUtils.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: John
 */


#include "TranslationData_t.h"s

#include "TranslateUtils.h"

using namespace std;

// TODO- Prepare items for translation/writing to file.
void TranslateUtils_prepareItems(std::vector<ABuildItem*> items, AnalysisData_t* model) {
	// Prepare translation data to be shared between items.
	TranslationData_t data;
	data.m_nextTextAddr = 0x0;
	data.m_nextDataAddr = 0x0;
	data.m_nextBssAddr = model->m_dataLen;

	//
}
