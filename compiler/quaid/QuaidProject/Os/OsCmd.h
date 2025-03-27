/*
 * OsCmd.h: Abstraction layer for using the command prompt/cli.
 */

#ifndef OS_OSCMD_H_
#define OS_OSCMD_H_

#include <string>
#include "Common/Domain/RetErr_e.h"

/*
 * @brief Runs the given command as a cli prompt. Is blocking.
 *
 * Blocks till command returns. Creates new process using the same "window"
 * as the calling program. Return indicates success of creating new process,
 * NOT result of command overall.
 *
 * @param cmd command to run in command prompt
 * @param res result of the executed command
 * @return RET_ERR_NONE if successful, RET_ERR_ERROR otherwise
 */
RetErr_e OsCmd_run(std::string& cmd, int& res);

#endif /* OS_OSCMD_H_ */
