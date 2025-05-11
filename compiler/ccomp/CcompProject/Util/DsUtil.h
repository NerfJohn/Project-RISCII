/*
 * DsUtil.h: Utilities regarding node/data structure operations.
 */

#ifndef UTIL_DSUTIL_H_
#define UTIL_DSUTIL_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Add symbol at current scope. (Re-)link if already strongly declared.
 *
 * Adds symbol if is does not exist. Performs "re-linking" if symbol exists, but
 * one or neither is a definition. Errors for two, defined symbols.
 *
 * @param model  data model of the entire program
 * @param name   name to add symbol under
 * @param newSym symbol to add (or possibly re-link)
 */
void DsUtil_addSym(DataModel_t& model, std::string& name, Ptr<Sym_t>& newSym);

// TODO
void DsUtil_cmpSyms(DataModel_t& model, Ptr<Sym_t>& symA, Ptr<Sym_t>& symB);

#endif /* UTIL_DSUTIL_H_ */
