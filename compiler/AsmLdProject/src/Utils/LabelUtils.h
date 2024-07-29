/*
 * LabelUtils.h
 *
 * "Helper functions for working with (special) labels"
 */

#ifndef SRC_UTILS_LABELUTILS_H_
#define SRC_UTILS_LABELUTILS_H_

#include <map>
#include <string>
#include "DomainLayer/DataModel_t.h"
#include "DomainLayer/LabelData_t.h"
#include "DomainLayer/RetErr_e.h"

/*
 * Initializes string->LabelData_t map with pre-defined label information.
 *
 * Adds entries to the given map that correspond to labels defined and/or used
 * by the assembler itself. Intended to be called by the LabelTable to
 * prepare it for use.
 *
 * @param table "table" object to add predefined labels to
 * @return zero if successful, non-zero otherwise
 */
RetErr_e LabelUtils_initPreDefs(std::map<std::string, LabelData_t>& table);

/*
 * Gets address associated with an assembler defined label.
 *
 * Computes address value for a given predefined value (these labels require
 * data about the program, NOT any given instruction). Return indicates if
 * address could be generated (ie if label if a predefined label).
 *
 * @param model data model of the program
 * @param label label to "convert" to an address
 * @param addr address generated from the label
 * @return zero if successful, non-zero otherwise (ie not a predefined label)
 */
RetErr_e LabelUtils_getPreDefAddr(DataModel_t const& model,
		                          std::string const label,
								  uint32_t& addr);

#endif /* SRC_UTILS_LABELUTILS_H_ */
