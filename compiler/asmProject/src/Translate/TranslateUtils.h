/*
 * TranslateUtils.h
 *
 *  Created on: Apr 7, 2024
 *      Author: John
 */

#ifndef SRC_TRANSLATE_TRANSLATEUTILS_H_
#define SRC_TRANSLATE_TRANSLATEUTILS_H_

#include <vector>
#include "ABuildItem.h"

// TODO- Prepare items for translation/writing to file.
void TranslateUtils_prepareItems(std::vector<ABuildItem*> items, AnalysisData_t* model);

#endif /* SRC_TRANSLATE_TRANSLATEUTILS_H_ */
