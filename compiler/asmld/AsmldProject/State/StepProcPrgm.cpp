/*
 * StepProcPrgm.cpp: State/step to process the program between input/output.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Ds/AAsmNode.h"
#include "Util/ModelUtil.h"

#include "State/StepProcPrgm.h"

using namespace std;

//==============================================================================
// Helper function to facilitate removing of unused labels/sections.
static void StepProcPrgm_removeLabels(DataModel_t& model) {
	// Log event.
	Print::inst().log(LOG_INFO, "Remove- removing unused labels");

	// Iterate over nodes until no more removes are left.
	bool didRemove = false;
	do {
		// Begin iteration with fresh perspective.
		didRemove = false;

		// Iterate over program- deleting nodes as applicable.
		vector<AAsmNode*>::iterator it = model.m_nodes.begin();
		while (it != model.m_nodes.end()) {
			// Get node.
			AAsmNode* node = *it;
			IF_NULL(node, "Cleaned null node");

			// Delete/remove as instructed.
			if (node->optRemoveLabel(model) == CLEAN_DELETE) {
				delete node;                  // delete node itself
				it = model.m_nodes.erase(it); // erase from list/move iterator
				didRemove = true;             // record-keep
			}
			else {/* otherwise, manual increment */ it++;}
		}

	} while(didRemove);
}

//==============================================================================
// Executes process to process overall program in model.
void StepProcPrgm_execute(DataModel_t& model) {
	// (Inform User.)
	Print::inst().log(LOG_INFO, "=Process Program=");

	// Ensure program's "anchor point" is defined correctly.
	Symbol_t* startSym = nullptr;
	if (model.m_gSyms.getSym(SYM_START_NAME, startSym) == RET_ERR_ERROR) {
		string errStr = string("Start label '") +
				        SYM_START_NAME          +
						"' must be defined";
		Print::inst().log(LOG_ERROR, errStr);
		ModelUtil_recordError(model, RET_NO_MAIN);
	}
	else if (startSym->m_space != ADDR_TEXT) {
		string errStr = string("Start label '")         +
		                SYM_START_NAME                  +
				        "' must define text section";
		Print::inst().log(LOG_ERROR, errStr);
		ModelUtil_recordError(model, RET_IN_TEXT);
	}

	// Complete primary checks/parsing of program.
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

	// Remove unused labels as requested.
	if (model.m_doRm) {StepProcPrgm_removeLabels(model);}

	// Terminate program (with summary) if errors were found.
	if (model.m_numErrs > 0) {Terminate_summary(model);}
}
