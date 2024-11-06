#ifndef TESTS_OS_OSFILE_H_
#define TESTS_OS_OSFILE_H_

#include <string>
#include "Domain/RetErr_e.h"

// Setup dummy read file (overwrites previous file(s)).
void OsFile_setFile(std::string name, std::string data);

// General setting to control how OsFile functions respond.
void OsFile_setResponse(RetErr_e resp);

// Get the dummy file's current name.
std::string OsFile_getName(void);

// Get the dummy file's curent data.
std::string OsFile_getData(void);

#endif /* TESTS_OS_OSFILE_H_ */