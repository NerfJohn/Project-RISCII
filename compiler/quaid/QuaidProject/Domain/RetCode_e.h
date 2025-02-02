/*
 * RetCode_e.h: Return codes for indicating reason for program termination.
 */

#ifndef DOMAIN_RETCODE_E_H_
#define DOMAIN_RETCODE_E_H_

/*
 * @brief Enum of return codes/values upon program termination.
 *
 * Values indicate unique reasons for termination. Intended to briefly indicate
 * reason for termination to caller.
 */
typedef enum {
	// Real values.
	RET_SUCCESS = 0x00, // Successful run of program
	RET_ASSERT  = 0x01, // Source code bug/error occurred

	// TODO- add program specific values- 1 per unique termination scenario
	// TODO- recommended to explictly state return code value per entry

} RetCode_e;

#endif /* DOMAIN_RETCODE_E_H_ */
