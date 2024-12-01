/*
 * StepProcPrgm.cpp: State/step to process the program between input/output.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Ds/AAsmNode.h"

#include "State/StepProcPrgm.h"

using namespace std;

// TODO
void StepProcPrgm_execute(DataModel_t& model) {
	// (Inform User.)
	Print::inst().log(LOG_INFO, "=Process Program=");

	// Complete primary checks/parsing or program.
	// TODO- global linking/checking
	for (AAsmNode* node : model.m_nodes) {
		if (node == nullptr) {Terminate_assert("Cleaned null node");}
		node->cleanProgram(model);
	}

	// Terminate program (with summary) if errors were found.
	if (model.m_numErrs > 0) {Terminate_summary(model);}
}
