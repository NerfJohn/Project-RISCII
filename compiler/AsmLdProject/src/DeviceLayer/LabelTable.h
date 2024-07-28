/*
 * LabelTable.h
 *
 * "Structure used to store and report observed assembly labels"
 */

#ifndef SRC_DEVICELAYER_LABELTABLE_H_
#define SRC_DEVICELAYER_LABELTABLE_H_

#include <map>
#include <string>
#include "DomainLayer/DataModel_t.h"
#include "DomainLayer/LabelData_t.h"
#include "DomainLayer/RetErr_e.h"

/*
 * Class to track and report on label information.
 *
 * Class records data taken about labels during program analysis, providing
 * analysis functions to report errors (done by class, though cannot terminate)
 * and extract inferred information about labels (e.g. resolved address).
 */
class LabelTable {
public:

	/*
	 * Defines the given label with a given data (if not already defined).
	 *
	 * Data given to define label is only used if label is not already defined.
	 * Function logs (but doesn't print) errors related to multiple definitions.
	 * If first definition of label, entry is created with given data.
	 *
	 * @param model data model of the program
	 * @param label label to define in the table
	 * @param data data to associate with the label
	 */
	void defineLabel(DataModel_t& model,
			         std::string const label,
					 LabelData_t const& data);

	/*
	 * Marks given label as used in the table. Creates entry as needed.
	 *
	 * Entry, if created by using label, is still considered "undefined" and
	 * (should) be defined via the class' defineLabel() function. No other
	 * entry records beyond the label being used are meaningfully saved.
	 *
	 * @param label name of the label to mark as used
	 */
	void useLabel(std::string const label);

	/*
	 * Sets the item associated with the given label. Label must exist.
	 *
	 * Label must have been defined or used for this function to have an
	 * effect.
	 *
	 * @param label label to associate item with
	 * @param item item to associate label to
	 */
	void setItem(std::string const label, IBuildItem& item);

	/*
	 * Runs checks on current entries. Reports any errors or warnings.
	 *
	 * Validates every label entry has been both defined and used. Any warnings
	 * or errors are logged and the model is updated accordingly.
	 *
	 * @param model data model of the program
	 */
	void validateTable(DataModel_t& model);

	/*
	 * Gets the generated address associated with the given label.
	 *
	 * Gets the address reported by the label's associated item. Returned value
	 * indicates if the address variable given was populated with the
	 * associated address.
	 *
	 * @param label label to 'convert' into an address
	 * @param addr location to put the address associated with the label
	 * @return zero if successful, non-zero otherwise
	 */
	RetErr_e getAddress(std::string const label, uint32_t& addr);

private:
	// Core data structure used to store label data.
	std::map<std::string, LabelData_t> m_table;
};

#endif /* SRC_DEVICELAYER_LABELTABLE_H_ */
