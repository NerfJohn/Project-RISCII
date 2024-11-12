#ifndef TESTS_OS_OSEXIT_H_
#define TESTS_OS_OSEXIT_H_

// Extract return code via OsExit.
int OsExit_getCode(void);

// Determine of program has returned.
bool OsExit_hasRet(void);

#endif /* TESTS_OS_OSEXIT_H_ */
