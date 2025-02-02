/*
 * main.cpp
 *
 *  Created on: Feb 1, 2025
 *      Author: John
 */

// TODO- format/fix

#include "version.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"

using namespace std;

int main(void) {
	// Setup.
	Print::inst().setLogLevel(LOG_INFO);
	Print::inst().setLogName("quaid.exe");

	// Some output.
	Print::inst().log(LOG_DEBUG, "foo", "JOHN CENA");
	Print::inst().log(LOG_ERROR, "f", 2, "blah");
	Print::inst().log(LOG_WARNING, APP_VERSION);

	// And out.
	DataModel_t foo = {
		.m_numWarns = 0,
		.m_numErrs = 0,
		.m_retCode = RET_SUCCESS
	};
	Terminate_summary(foo);


	return RET_SUCCESS;
}
