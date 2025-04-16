/*
 * Terminate.h: Standardizes program exit scenarios.
 */

#ifndef COMMON_DEVICE_TERMINATE_H_
#define COMMON_DEVICE_TERMINATE_H_

/*
 * @brief Sets "bug" exit code for Terminate_bug().
 *
 * Same code is used for any call of Terminate_bug(). Can be set multiple times.
 *
 * @param exitCode return code to use for Terminate_bug().
 */
void Terminate_setBugCode(int const bugCode);

/*
 * @brief Terminates program (without output besides given return code).
 *
 * @param exitCode value to return to caller (of program, NOT function)
 */
void Terminate_silent(int const exitCode);

/*
 * @brief Terminates program using provided bug code.
 *
 * Intended to allow "Common" functions assert without knowing the exact code
 * to use. Code is determined by application calling Terminate_setBugCode().
 */
void Terminate_bug(void);

#endif /* COMMON_DEVICE_TERMINATE_H_ */
