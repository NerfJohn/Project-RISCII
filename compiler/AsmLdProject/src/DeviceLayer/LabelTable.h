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
	 * TODO
	 */
	void defineLabel(DataModel_t& model,
			         std::string const label,
					 LabelData_t const& data);

	/*
	 * TODO
	 */
	void useLabel(std::string const label);

	/*
	 * TODO
	 */
	void setItem(std::string const label, IBuildItem& item);

	/*
	 * TODO
	 */
	void validateTable(DataModel_t& model);

private:
	// Core data structure used to store label data.
	std::map<std::string, LabelData_t> m_table;
};

#endif /* SRC_DEVICELAYER_LABELTABLE_H_ */
