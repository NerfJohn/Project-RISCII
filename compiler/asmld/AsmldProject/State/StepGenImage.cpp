/*
 * StepGenImage.cpp: State/step to generate binary image from parsed program.
 */

#include "Device/Terminate.h"
#include "Ds/AAsmNode.h"
#include "Util/TargetUtil.h"

#include "State/StepGenImage.h"

using namespace std;

//==============================================================================
// TODO
void StepGenImage_execute(DataModel_t& model) {
	// First assemble the program.
	for (AAsmNode* node : model.m_nodes) {
		if (node == nullptr) {Terminate_assert("Assembled null node");}
	}

	// (Ensure target requirements are met.)
	if (model.m_dataSize < TARGET_SECT_MIN_BYTES) {

	}

	// Perform final checks on whether program will fit on target.

}

