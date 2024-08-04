/*
 * ReadFile_t.h
 *
 * "File stream dedicated explicitly to reading the file"
 */

#ifndef SRC_DOMAINLAYER_READFILE_T_H_
#define SRC_DOMAINLAYER_READFILE_T_H_

#include <fstream>

/*
 * File that can only be read. Intended to be used to access input files.
 */
typedef std::fstream ReadFile_t;

#endif /* SRC_DOMAINLAYER_READFILE_T_H_ */
