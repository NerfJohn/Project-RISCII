/*
 * CleanAction_e.h: Enum defining action to take while cleaning up nodes.
 */

#ifndef DOMAIN_CLEANACTION_E_H_
#define DOMAIN_CLEANACTION_E_H_

/*
 * Enum defining actions that can be taken to "clean-up" node.
 *
 * Intended as a return type from a node to a calling state step to allow for
 * the state to handle clean-up (vs node deleting/removing itself from the
 * program).
 */
typedef enum {
	// Real values.
	CLEAN_KEEP,              // keep/maintain the specific node
	CLEAN_DELETE             // delete the specific node
} CleanAction_e;

#endif /* DOMAIN_CLEANACTION_E_H_ */
