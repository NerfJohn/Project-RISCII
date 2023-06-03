# Parse Table Generator: For use for building software
# by John Eslinger

import os
import re
import sys

"""
# Parse Table Script Ruleset
# ==========================
# 1) Format: <cur_state>::<token>::<new_states>
# 2) Sub states use lower_snake_case, tokens use UPPER_SNAKE_CASE
# 3) New states specified via CSV format (in order of expansion)
# 4) Prefix '#' to indicate actions, use UPPER_SNAKE_CASE
# 5) "prgm" state and "EOF" token are built-in
# ==========================
# Note: Tokens based off enum created in ScanTable.h
# Note: Leave new states blank to indicate an epsilon
"""

# Top "constant" of the file to create.
FILE_TOP = '\n'.join([
            "/*",
            " * ParseTable.h (auto generated by genParseTable.py).",
            " */",
            "",
            "#ifndef PARSETABLE_H_",
            "#define PARSETABLE_H_",
            "",
            "#include <stdint.h>",
            "#include <vector>",
            "#include \"ScanTable.h\"",
            "",
            "/*",
            " * Enum defining key states for parse table use.",
            " *",
            " * Most states reflect \"actions\", which signal that enough tokens have been",
            " * parsed to create a node. A starting state is provided for resetting state",
            " * variables as needed, along with a few \"sentinel\" values for ease of use.",
            " */",
            "enum ParseActions {",
            "",
            ])
 
# Middle "constant" of the file to create.
FILE_MIDDLE = '\n'.join([
                "};",
                "",
                "/*",
                " * Static class used to contain parse table for easy access.",
                " *",
                " * Class provides function/buffer to simplify state transitions for parsing. The",
                " * function uses a pointer to infer new states to process (ie valid pointer to",
                " * a buffer) or a bad state transition(ie nullptr).",
                " *",
                " * Similar to ScanTable.h, the states are implemented using uint8_t. For",
                " * ParseTable.h, its states act as a \"superset\" of ScanTable.h's states, where",
                " * enums ScanTableStates and ParseActions meaningfully describe the states' use.",
                " *",
                " */",
                "class ParseTable {",
                "public:",
                "    /*",
                "     * Translates state and token to list of new states.",
                "     *",
                "     * Function simplifies transition logic of parse table to a function. Using",
                "     * a private buffer, the function returns a pointer to the buffer to infer",
                "     * new states to be processed (or a null pointer to infer a bad transition).",
                "     *",
                "     * @param curState current state of the parser",
                "     * @param token current token (type) being parsed",
                "     * @return pointer to buffer of states if valid transition, null otherwise",
                "     */",
                "    static std::vector<uint8_t>* getNextStates(uint8_t curState,",
                "                                               ScanTableStates token) {",
                "        // Use constant to acknowledge \"EOF\" tokens.",
                "        const uint8_t tknEOF = -1;",
                "",
                "        // Assume we'll return our (emptied) state buffer for now.",
                "        static std::vector<uint8_t> s_nextStates = {};",
                "        std::vector<uint8_t>* retPtr = &s_nextStates;",
                "",
                "        // Use unique \"identifier\" to determine next states.",
                "        switch((curState << 8) | (0xff & token)) {",
                "",
                ])

# End "constant" of the file to create.
FILE_END = '\n'.join([
            "            default:",
            "                // No matching transition- nullify ptr to signal this.",
            "                retPtr = nullptr;",
            "        }",
            "",
            "        // Return the new array of states (or lack thereof).",
            "        return retPtr;",
            "    }",
            "",
            "    /*",
            "     * Std. Destructor.",
            "     */",
            "    virtual ~ParseTable(void) {/* Empty dtor */}",
            "",
            "private:",
            "    // Private constructor to prevent instantiation.",
            "    ParseTable(void) {/* Empty ctor */}",
            "};",
            "",
            "#endif /* PARSETABLE_H_ */",
            ])

# Built-in states of the scan table.
PARSE_START = "prgm"
PARSE_NODE_MIN = "PARSE_NODE_MIN"
PARSE_NODE_MAX = "PARSE_NODE_MAX"
EOF = "EOF"

# Files to create.
TABLE_FILE = "ParseTable.h"

# Function to generate the resulting header file (for source code).
def genTableFile(table, states):
    # Create/overwrite header file.
    file = open(TABLE_FILE, "w")
    
    # Write the top portion of the file.
    file.write(FILE_TOP)
    
    # Write the enum to the file (conveniently in the order they were assigned- incrementally).
    file.write("\tPARSE_START = %d,\n"%states[PARSE_START])
    for state in states:
        if (state == PARSE_START or states[state] < states[PARSE_NODE_MIN]): continue
        enumName = state.replace('#', '') # Remove undesired hashtags
        file.write("\t%s = %d,\n"%(enumName, states[state]))
    
    # Write the middle portion of the file.
    file.write(FILE_MIDDLE)
    
    # Write "table" to the file.
    for entry in table:
        file.write("\t\t\tcase %s: s_nextStates = %s; break;\n"%(entry, table[entry]))
    
    # Write end portion of the file.
    file.write(FILE_END)
    
    # Close finished file.
    file.close()

# Main Execution.
if __name__ == "__main__":
    # Serve help flag.
    if (len(sys.argv) == 2) and (sys.argv[1] == "-h"):
        print("python genParseTable.py [-h] <input mock file>")
        sys.exit(0)
    
    # Check file is provided.
    if (len(sys.argv) < 2) or not os.path.isfile(sys.argv[1]):
        print("Existing file not provided!")
        sys.exit(1)
    
    # Prepare to keep track of file information.
    subStates = {PARSE_START:0}
    actStates = {}
    rules = {}
    
    # Parse file.
    lines = open(sys.argv[1], 'r').readlines()
    for line in lines:
        # Chop off newline.
        line = line.replace('\n', '')
        
        # Ignore comments and spacers.
        if (len(line) == 0 or line[0] == '#'): continue
        
        # Ensure rule matches expected format.
        if (not re.fullmatch(r'[_a-z]+::[_A-Z]+::([#,_a-zA-Z]*|--)', line)):
            print("Unrecognized format: %s"%line)
            sys.exit(1)
        
        # Record rule data.
        parts = line.split("::")
        tkns = parts[2].split(",")
        subStates[parts[0]] = 0
        for tkn in tkns:
            # Parse new states- sub and action.
            if (re.fullmatch(r'[_a-z]+', tkn)): subStates[tkn] = 0
            if (re.fullmatch(r'#[_A-Z]+', tkn)): actStates[tkn] = 0
        rules[parts[0] + "," + parts[1]] = parts[2]
    
    # Consolidate states and check their numbers.
    allStates = {}
    num = 0
    for state in subStates:
        allStates[state] = num
        num += 1
    if (num >= 100):
        print("Too many substates for scheme! Refactor design and/or generator!")
        sys.exit(1)
    num = 200
    allStates[PARSE_NODE_MIN] = num
    for state in actStates:
        allStates[state] = num
        num += 1
    allStates[PARSE_NODE_MAX] = num - 1
    if (num >= 255):
        print("Too many action states for scheme! Refactor design and/or generator!")
        sys.exit(1)
    
    # Create "table" by converting rules into list of new states.
    table = {}
    for rule in rules:
        # First determine the rule's key- (state << 8) | (0xff & token).
        parts = rule.split(",")
        if parts[0] in subStates: parts[0] =  ''.join("0x{:02x}".format(allStates[parts[0]]))
        if parts[1] == EOF: parts[1] = "tknEOF"
        key = "(" + parts[0] + " << 8) | (0xff & " + parts[1] + ")"
        
        # Next determine the rule's outcome- a list of tokens.
        outcome = "{" + parts[1]
        parts = rules[rule].split(",")
        for part in parts:
            if (len(part) > 0): outcome += ","
            if part in subStates: outcome += ''.join("0x{:02x}".format(allStates[part]))
            elif part in actStates: outcome += part[1:] # Remove leading hashtag
            else: outcome += part
        outcome = outcome + "}"
        
        # Clear out epsilons that are intended to be completely clear.
        if len(parts) == 1 and parts[0] == "--": outcome = "{}"
        
        # Add to table.
        table[key] = outcome
    
    # Generate files.
    genTableFile(table, allStates)