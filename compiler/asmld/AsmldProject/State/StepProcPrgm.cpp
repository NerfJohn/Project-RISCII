/*
 * StepProcPrgm.cpp: State/step to process the program between input/output.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Ds/AAsmNode.h"

#include "State/StepProcPrgm.h"

using namespace std;

// Executes process to process overall program in model.
void StepProcPrgm_execute(DataModel_t& model) {
	// (Inform User.)
	Print::inst().log(LOG_INFO, "=Process Program=");

	// Complete primary checks/parsing or program.
	for (AAsmNode* node : model.m_nodes) {
		IF_NULL(node, "Globally linked null node");
		node->globalLink(model);
	}

	// Clean up program- deleting nodes as applicable.
	vector<AAsmNode*>::iterator it = model.m_nodes.begin();
	while (it != model.m_nodes.end()) {
		// Get node.
		AAsmNode* node = *it;
		IF_NULL(node, "Cleaned null node");

		// Delete/remove as instructed.
		if (node->globalClean(model) == CLEAN_DELETE) {
			delete node;                  // delete node itself
			it = model.m_nodes.erase(it); // erase from list/move iterator
		}
		else {/* otherwise, manual increment */ it++;}
	}

	// Terminate program (with summary) if errors were found.
	if (model.m_numErrs > 0) {Terminate_summary(model);}
}
