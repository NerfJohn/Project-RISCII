/*
 * Cmd.h: Standardized method of calling command line.
 */

#ifndef DEVICE_CMD_H_
#define DEVICE_CMD_H_

#include <string>

/*
 * @brief Calls command on command line. Is blocking.
 *
 * Blocks until finished running. Assumes command is known to work ahead of time
 * (ie errors result from called program, NOT the call itself).
 *
 * @param cmd command to run in command line
 * @return    return code of the called program
 */
int Cmd_run(std::string cmd);

#endif /* DEVICE_CMD_H_ */
