/*
 * CliFlag_e.h: Specific flags that can be passed to irbend.exe.
 */

#ifndef DOMAIN_CLIFLAG_E_H_
#define DOMAIN_CLIFLAG_E_H_

/*
 * Enum defining the flags recognized by irbend.exe.
 *
 * These flags can be used to specifically identify certain flags (and compare
 * requirements for flag arguments based on the flag). A special "invalid"
 * value is provided for "no (valid) value" scenarios.
 */
typedef enum {
	// Real values.
	CLI_FLAG_HELP,     // request program help menu
	CLI_FLAG_NAME,     // name output file

	// Special values.
	CLI_FLAG_INVALID   // invalid/unused flag

} CliFlag_e;

#endif /* DOMAIN_CLIFLAG_E_H_ */
