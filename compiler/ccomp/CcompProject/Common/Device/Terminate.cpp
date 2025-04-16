/*
 * Terminate.cpp: Standardizes program exit scenarios.
 */

#include <string>
#include "Common/Os/OsExit.h"

#include "Common/Device/Terminate.h"

using namespace std;

//==============================================================================

// Local save of exit code settings.
static int s_bugCode = -1; // (dummy value- avoid 0 as common "success" code)

//==============================================================================
// Sets "bug" exit code for Terminate_bug().
void Terminate_setBugCode(int const bugCode) {s_bugCode = bugCode;}

//==============================================================================
// Terminates program (without output besides given return code).
void Terminate_silent(int const exitCode) {OsExit_exit(exitCode);}

//==============================================================================
// Terminates program using provided bug code.
void Terminate_bug(void) {OsExit_exit(s_bugCode);}
