/*
 * IBuildItem.h
 *
 * "Interface defining necessary functions per assembly construct"
 */

#ifndef SRC_ITEMS_IBUILDITEM_H_
#define SRC_ITEMS_IBUILDITEM_H_

/*
 * Interface used to define all assembly language constructs/larger pieces.
 *
 * Interface allows program to store, iterate over, and call the items that
 * ultimately define the high-level assembly language features. The interface
 * primarily defines the necessary functions to check and translate items.
 */
class IBuildItem {
public:
	/*
	 * Std. destructor- no real use for interface.
	 */
	~IBuildItem(void) {}
};

#endif /* SRC_ITEMS_IBUILDITEM_H_ */
