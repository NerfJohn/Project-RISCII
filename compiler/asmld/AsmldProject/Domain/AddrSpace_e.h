/*
 * AddrSpace_e.h: Enum defining different address spaces labels can belong to.
 */

#ifndef DOMAIN_ADDRSPACE_E_H_
#define DOMAIN_ADDRSPACE_E_H_

/*
 * Enum defining address spaces a label/address can refer to.
 */
typedef enum {
	// Core values.
	ADDR_TEXT,               // text/program address space
	ADDR_DATA,               // init data (in data address space)
	ADDR_BSS,                // un-init data (in data address space)

	// Special values.
	ADDR_INVALID             // invalid/unknown address space
} AddrSpace_e;

#endif /* DOMAIN_ADDRSPACE_E_H_ */
