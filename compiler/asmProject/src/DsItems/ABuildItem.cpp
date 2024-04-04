/*
 * ABuildItem.cpp
 *
 *  Created on: Apr 1, 2024
 *      Author: John
 */


#include "ABuildItem.h"

using namespace std;

// TODO- read out file/line origin (namely for reporting).
std::string ABuildItem::getOrigin(void) {
	// Simple getter.
	return m_origin;
}

// TODO- Std. destructor.
ABuildItem::~ABuildItem(void) {/*No non-std pointers*/}
