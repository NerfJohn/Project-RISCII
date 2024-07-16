/*
 * WriteFile_t.h
 *
 * "File stream dedicated to writing to a file"
 */

#ifndef SRC_DOMAINLAYER_WRITEFILE_T_H_
#define SRC_DOMAINLAYER_WRITEFILE_T_H_

#include <fstream>

/*
 * File that can only be written. Intended to be used to create image files.
 */
typedef std::fstream WriteFile_t;

#endif /* SRC_DOMAINLAYER_WRITEFILE_T_H_ */
