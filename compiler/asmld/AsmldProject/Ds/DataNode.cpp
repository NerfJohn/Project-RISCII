/*
 * DataNode.cpp: Represents directives specifying global data (data and bss).
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/IsaUtil.h"
#include "Util/ModelUtil.h"

#include "Ds/DataNode.h"

using namespace std;

//==============================================================================

// Definitions for recognized escape chars.
#define ESC_NULL       ('0')
#define ESC_BSLASH     ('\\')
#define ESC_QUOTE      ('"')
#define ESC_NEWLINE    ('n')
#define ESC_RETURN     ('r')
#define ESC_TAB        ('t')

//==============================================================================
// Helper function to determine bytes to allocate using immediate.
uint32_t DataNode::allocImm(DataModel_t& model,
		                    ItemToken const& imm,
				            ItemToken const& type) {
	// Number of bytes to allocate/return.
	uint32_t retNum = 0;

	// Allocation depends on type of directive.
	switch (type.m_lexTkn) {
		case TOKEN_KW_DATA: retNum += ISA_WORD_BYTES;                 break;
		case TOKEN_KW_BSS:  retNum += this->getImm(model, imm, type); break;
		default:
			// Unknown directive type. Bug!
			Terminate_assert("allocImm() unknown directive");
	}

	// Inform user if allocation will be padded.
	if (retNum % ISA_WORD_BYTES) {
		// Log event.
		string infStr = string("Padding reservation for alignment");
		Print::inst().log(LOG_INFO, imm.m_file, imm.m_line, infStr);

		// Adjust count.
		while (retNum % ISA_WORD_BYTES) {retNum++;}
	}

	// Return resulting number of bytes to allocate.
	return retNum;
}

//==============================================================================
// Helper function to validate/extract escape chars.
RetErr_e DataNode::asEscape(uint8_t& chr) {
	// Result of the process (optimistic).
	RetErr_e retErr = RET_ERR_NONE;

	// Adjust given char to it's equivalent escape value.
	switch (chr) {
		case ESC_NULL:    chr = '\0'; break;
		case ESC_BSLASH:  chr = '\\'; break;
		case ESC_QUOTE:   chr = '"';  break;
		case ESC_NEWLINE: chr = '\n'; break;
		case ESC_RETURN:  chr = '\r'; break;
		case ESC_TAB:     chr = '\t'; break;
		default:
			// No matching escape- failure.
			retErr = RET_ERR_ERROR;
	}

	// Return result of process.
	return retErr;
}

//==============================================================================
// Helper function to validate string literal (more so escape chars).
void DataNode::validateStr(DataModel_t& model, ItemToken const& str) {
	// Flag to track when char is escaped.
	bool isEsc = false;

	// Check bytes for escape chars to validate.
	for (uint8_t byte : str.m_rawStr) {
		// Record unknown escape chars.
		if (isEsc && (this->asEscape(byte) == RET_ERR_ERROR)) {
			string errStr = string("Unknown escape '\\") + (char)(byte) + "'";
			Print::inst().log(LOG_ERROR, str.m_file, str.m_line, errStr);
			ModelUtil_recordError(model, RET_BAD_ESC);
		}

		// Handle finding escapes.
		if (isEsc) {isEsc = false;}
		else       {isEsc = (byte == ESC_BSLASH);}
	}
}

//==============================================================================
// Helper function to determine bytes to allocate for string literal.
uint32_t DataNode::allocStr(DataModel_t& model, ItemToken const& str) {
	// Number of bytes to allocate/return.
	uint32_t retNum = 0;

	// Flag to track when char is escaped.
	bool isEsc = false;

	// Count each byte allocated by the string literal.
	for (uint8_t byte : str.m_rawStr) {
		// Count bytes- with 1 byte escapes and ignoring enclosing quotes
		if      (isEsc)             {isEsc = false;}
		else if (byte == ESC_QUOTE) {/* don't count */}
		else                        {retNum++; isEsc = (byte == ESC_BSLASH);}
	}

	// Inform user if string will get padded during assembly.
	if (retNum % ISA_WORD_BYTES) {
		// Log event.
		string infStr = string("Padding ")          +
				        str.m_rawStr                +
						" with NULL for alignment";
		Print::inst().log(LOG_INFO, str.m_file, str.m_line, infStr);

		// Adjust count.
		while (retNum % ISA_WORD_BYTES) {retNum++;}
	}

	// Return resulting number of bytes to allocate.
	return retNum;
}

//==============================================================================
// Helper function to translate string literal into data section.
void DataNode::genStr(DataModel_t& model, ItemToken const& str) {
	// Flag to track when char is escaped.
	bool isEsc        = false;

	// String to contain extracted byte values.
	string rawBytes = "";

	// Remove surrounding quotes.
	string strLit = str.m_rawStr.substr(1, str.m_rawStr.size() - 2);

	// Parse literal into its raw bytes.
	for (uint8_t byte : strLit) {
		// Add each byte- adding converted escape as needed.
		if      (isEsc)              {
			this->asEscape(byte);
			rawBytes += byte;
			isEsc = false;
		}
		else if (byte == ESC_BSLASH) {isEsc = true;}
		else                         {rawBytes += byte;}
	}

	// Add padding.
	while (rawBytes.size() % ISA_WORD_BYTES) {rawBytes += (char)(NULL);}

	// Add to model- storing LOWER address bytes at LOWER bits.
	for (size_t i = 0; (i + 1) < rawBytes.size(); i += 2) {
		uint16_t word = (rawBytes[i + 1] << 8) | rawBytes[i];
		model.m_dataVals.push_back(word);
	}
}

//==============================================================================
// Constructor called by parser. Builds itself directly from action stack.
DataNode::DataNode(std::stack<ItemToken*>& itemStack) {
	// (Init members.)
	m_reqType = nullptr;
	m_optVals = {};
	m_syms    = {};

	// Parse all items from the stack.
	while (itemStack.size() > 0) {
		// Get next item.
		ItemToken* item = itemStack.top();
		IF_NULL(item, "DataNode() with null item");

		// "Take" item in appropriate spot (REALLY trusting parser, here).
		switch (item->m_lexTkn) {
			case TOKEN_STRLIT:
			case TOKEN_LABEL:
			case TOKEN_IMMEDIATE:    m_optVals.push_front(item); break;
			case TOKEN_LCURLY:
			case TOKEN_RCURLY:       delete item;                break;
			default: /* directive */ m_reqType = item;           break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// Ensure required items are present.
	IF_NULL(m_reqType, "DataNode() without directive");

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
			          m_reqType->m_file,
					  m_reqType->m_line,
					  "DataNode created");
}

//==============================================================================
// Analyzes node- validating local arguments/symbols.
void DataNode::localAnalyze(DataModel_t& model, SymTable& table){
	// Validate each item.
	for (ItemToken* item : m_optVals) {
		// Validate based on underlying type.
		IF_NULL(item, "analyze() null data item");
		switch(item->m_lexTkn) {
			case TOKEN_STRLIT:
				// String literal- validate escapes.
				this->validateStr(model, *item);
				break;
			case TOKEN_LABEL:
				// Label- no actions to take as of yet.
				break;
			case TOKEN_IMMEDIATE:
				// Immediate- use common checker.
				this->getImm(model, *item, *m_reqType); // ignore val for now
				break;
			default:
				// Unknown type made it through parsing? bug!
				Terminate_assert("analyze() unknown data item");
				break;
		}
	}

	// Claim any open labels- using appropriate space.
	AddrSpace_e space = ADDR_INVALID;
	switch(m_reqType->m_lexTkn) {
		case TOKEN_KW_DATA: space = ADDR_DATA; break;
		case TOKEN_KW_BSS:  space = ADDR_BSS;  break;
		default: Terminate_assert("analyze() unknown data directive"); break;
	}
	this->pairLabels(model, *m_reqType, space);

	// Update model for "naturally" occurring data section.
	if (space == ADDR_DATA) {model.m_dataNodes++;}
}

//==============================================================================
// Handles local links/symbols- modifying and linking to local symbols.
void DataNode::localLink(DataModel_t& model, SymTable& table) {
	// Create symbol links for each label present.
	for (ItemToken* item : m_optVals) {
		// Create/link symbol for label.
		IF_NULL(item, "localLink() with null data item");
		if (item->m_lexTkn == TOKEN_LABEL) {
			Symbol_t* sym = nullptr;            // create
			this->linkLocal(table, *item, sym); // link
			m_syms.push_back(sym);              // save
		}
	}
}

//==============================================================================
// Handles global links/symbols- finishing overall symbol linkage.
void DataNode::globalLink(DataModel_t& model) {
	// Counter to track symbol currently being worked on.
	size_t symIdx = 0;

	// Link remaining labels within the itms.
	for (ItemToken* item : m_optVals) {
		// Link label.
		IF_NULL(item, "globalLink() with null data item");
		if (item->m_lexTkn == TOKEN_LABEL) {
			// Link (as needed- function skips solid links).
			this->linkGlobal(model, *item, m_syms[symIdx]);

			// To next symbol to update.
			symIdx++;
		}
	}
}

//==============================================================================
// Analyze program- generating addresses for each symbol.
void DataNode::imageAddress(DataModel_t& model) {
	// Total byte size of node.
	uint32_t totSize = 0;

	// Counter to track symbol currently being worked on.
	size_t symIdx = 0;

	// Accumulate the size of each item.
	for (ItemToken* item : m_optVals) {
		// Get size based off underlying type.
		IF_NULL(item, "address() null data item");
		switch (item->m_lexTkn) {
			case TOKEN_STRLIT:
				// String literal- determined by length of realized string.
				totSize += this->allocStr(model, *item);
				break;
			case TOKEN_LABEL:
				// Label- ALWAYS word size.
				totSize += ISA_WORD_BYTES;
				symIdx++;
				break;
			case TOKEN_IMMEDIATE:
				// Immediate- size based on directive type.
				totSize += this->allocImm(model, *item, *m_reqType);
				break;
			default:
				// Unknown type made it past parsing? bug!
				Terminate_assert("address() unknown data item");
				break;
		}
	}

	// Save size to proper location.
	switch(m_reqType->m_lexTkn) {
		case TOKEN_KW_DATA: model.m_dataSize += totSize;               break;
		case TOKEN_KW_BSS:  model.m_bssSize  += totSize;               break;
		default: Terminate_assert("address() unknown data directive"); break;
	}

	// Warn if no memory was allocated.
	if (totSize == 0) {
		Print::inst().log(LOG_WARNING,
				          m_reqType->m_file,
						  m_reqType->m_line,
						  "No memory allocated");
		ModelUtil_recordWarn(model);
	}
}

//==============================================================================
// Assembles program- generating binary values in the data model.
void DataNode::imageAssemble(DataModel_t& model) {
	// Handy uint16_t for intermediate computing.
	uint16_t binData = 0;

	// Counter to track symbol currently being worked on.
	size_t symIdx = 0;

	// Only initialized data creates image data.
	if (m_reqType->m_lexTkn == TOKEN_KW_DATA) {
		// Assemble each item.
		for (ItemToken* item : m_optVals) {
			// Translate based on underlying type.
			IF_NULL(item, "assemble()null data item");
			switch (item->m_lexTkn) {
				case TOKEN_STRLIT:
					// String literal- add realized bytes.
					this->genStr(model, *item);
					break;
				case TOKEN_LABEL:
					// Label- add resolved address.
					IF_NULL(m_syms[symIdx], "assemble() null symbol data item");
					binData = this->getAddr(model, m_syms[symIdx]);
					model.m_dataVals.push_back(binData);
					symIdx++;
					break;
				case TOKEN_IMMEDIATE:
					// Immediate- extract/save word.
					binData = (uint16_t)this->getImm(model, *item, *m_reqType);
					model.m_dataVals.push_back(binData);
					break;
				default:
					// Unknown type made it past parsing? Bug!
					Terminate_assert("assemble() unknown data item");
			}
		}
	}
}

//==============================================================================
// Determines if node (+related section nodes) should be removed.
CleanAction_e DataNode::optRemoveLabel(DataModel_t& model) {
	// Result of the process.
	CleanAction_e retAct = CLEAN_KEEP;

	// Return is based on underlying data type.
	switch (m_reqType->m_lexTkn) {
		case TOKEN_KW_DATA:
			retAct = (model.m_rmData) ? CLEAN_DELETE : CLEAN_KEEP;
			if (retAct == CLEAN_DELETE) {model.m_dataNodes--;}    // to delete
			break;
		case TOKEN_KW_BSS:
			retAct = (model.m_rmBss) ? CLEAN_DELETE : CLEAN_KEEP;
			break;
		default:
			Terminate_assert("removeLabel() with bad data type");
	}

	// Return result of the process.
	return retAct;
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
DataNode::~DataNode(void) {
	// Free required items.
	delete m_reqType;

	// Free other items as necessary.
	for (ItemToken* item : m_optVals) {
		if (item == nullptr) {delete item;}
	}
	for (Symbol_t* sym : m_syms)      {
		if (sym != nullptr) {this->freeSymbol(sym);}
	}
}

