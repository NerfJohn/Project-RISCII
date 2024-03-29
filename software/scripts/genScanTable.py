# Scan Table Generator: For use for building software
# by John Eslinger

import os
import re
import sys

"""
# Scan Table Script Ruleset
# =========================
# 1) Format: <cur_state>::<regex>::<new_state>
# 2) Sub states use lower_snake_case, end states use UPPER_SNAKE_CASE
# 3) Regexes match to string length of 1 (i.e. avoid ?, use [], etc)
# 4) Append '*' to end states that shouldn't include last parsed char
# 5) "start" and "SCAN_ERROR" states are built in
# =========================
# Note: Rules parsed in order- allows for "chaining" rules together
# Note: End states denote token creation/action, NOT "regex match"
"""

# Top "constant" of the file to create.
FILE_TOP = '\n'.join([
            "/*",
            " * ScanTable.h (auto generated by genScanTable.py).",
            " */",
            "",
            "#ifndef SCANTABLE_H",
            "#define SCANTABLE_H",
            "",
            "#include <regex>",
            "#include <stdint.h>",
            "",
            "/*",
            " * Enum defining key states for scan table use.",
            " *",
            " * Most states reflect \"end states\" that signal a specific token should be",
            " * created. A start state is provided for resetting state variables as needed,",
            " * along with three \"sentinel enum states\" for easily distinguishing end states",
            " * and when the last read character should be part of the new token.",
            " */",
            "",
            ])

# Middle "constant" of the file to create.
FILE_MIDDLE = '\n'.join([
                "",
                "/*",
                " * Static class used to contain scan table for easy access.",
                " *",
                " * Class provides an easy way to implement scan table logic for token",
                " * recognition. It's public functions allow for easy checking/translating",
                " * of valid chars into new states.",
                " */",
                "class ScanTable {",
                "public:",
                "    /*",
                "     * Checks if given char is part of the scan table's language.",
                "     *",
                "     * Function gives an idea how ScanTable::getNextState() will respond for a",
                "     * given input char without having to guess if the char or state transition",
                "     * caused a SCAN_ERROR.",
                "     *",
                "     * @param inChar the character being checked for validity",
                "     * @return true if char is valid for the scan table, false otherwise",
                "     */",
                "    static bool isValidChar(const char inChar) {",
                "        // Match the char against the scan table's regex.",
                "        std::regex scanTableRegex(\"[ -~\\t\\n]\");",
                "        if (inChar == -1 /*EOF*/) {return true;} // Special case 'char'",
                "        return std::regex_match(&inChar, scanTableRegex);",
                "    }",
                "",
                "    /*",
                "     * Translates state and character into new state.",
                "     *",
                "     * Function simplifies transition logic of scan table to a function. The",
                "     * function takes care of accessing table and returns the new state.",
                "     *",
                "     * @param curState state to start transition from",
                "     * @param nextChar character to transition with",
                "     * @return resulting state from given state/char pair",
                "     */",
                "    static uint8_t getNextState(const uint8_t curState, const char nextChar) {",
                "",
                ]) 

# End "constant" of the file to create.
FILE_END = '\n'.join([
            "",
            "        // Translate char to scan table index.",
            "        int colIdx = nextChar - (int)(' '); // Assume valid char",
            "        if (nextChar == '\\t') {colIdx = 95;}",
            "        else if (nextChar == '\\n') {colIdx = 96;}",
            "        else if (nextChar == -1 /*EOF*/) {colIdx = 97;}",
            "        else if (nextChar >= '\\x7F') {colIdx = -1;}",
            "",
            "        /* colIdx now either valid or -1. */",
            "",
            "        // Return new state (SCAN_ERROR if access is invalid).",
            "        if (colIdx < 0) {return SCAN_ERROR;}",
            "        else if (curState >= sizeof(s_TABLE)) {return SCAN_ERROR;}",
            "        else {return s_TABLE[curState][colIdx];}",
            "    }",
            "",
            "    /*",
            "     * Std. Destructor.",
            "     */",
            "    virtual ~ScanTable(void) {/* Empty dtor */}",
            "",
            "private:",
            "    // Private constructor to prevent instantiation.",
            "    ScanTable(void) {/* Empty ctor */}",
            "};",
            "",
            "#endif /* SCANTABLE_H_ */",
            ])

# Built-in states of the scan table
SCAN_START = "start"
SCAN_ERROR = "SCAN_ERROR"
SCAN_END_MIN = "SCAN_END_MIN"
SCAN_EXCLUDE_MIN = "SCAN_EXCLUDE_MIN*" # Added '*' to fit in with other "exclude" end states
SCAN_END_MAX = "SCAN_END_MAX"

# "Visible ASCII" characters: [space] through '~', plus \t,\n, and EOF = 98 characters in language.
LANG_NUM_CHARS = 98

# Files to create.
TABLE_FILE = "ScanTable.h"

# Quick converter to account for odd character/index relationship.
def colIdxToChar(colIdx):
    # Apply general rules of indices.
    retChar = colIdx + ord(' ')
    if (colIdx == 95): retChar = ord('\t')
    if (colIdx == 96): retChar = ord('\n')
    if (colIdx == 97): retChar = ord('\xFF')
    
    # Return as character.
    return chr(retChar)

# Function to generate the resulting header file (for source code).
def genTableFile(table, states):
    # Create/overwrite header file.
    file = open(TABLE_FILE, "w")
    
    # Write the top portion of the file.
    file.write(FILE_TOP)
    
    # Write the enum to the file (conveniently in the order they were assigned- incrementally).
    file.write("enum ScanTableStates {\n")
    file.write("\tSCAN_START = %d,\n"%states[SCAN_START])
    for state in states:
        if (state == SCAN_START or states[state] < states[SCAN_END_MIN]): continue
        enumName = state.replace('*', '') # Remove undesired asterisks
        file.write("\t%s = %d,\n"%(enumName, states[state]))
    file.write("};\n")
    
    # Write the middle portion of the file.
    file.write(FILE_MIDDLE)
    
    # Write the scan table to the file.
    file.write("\t\tstatic const uint8_t s_TABLE[%d][%d] = {\n"%(len(table), LANG_NUM_CHARS))
    for row in range(len(table)):
        file.write("\t\t\t{")
        for col in range(LANG_NUM_CHARS):
            file.write("0x%02x,"%int(table[row][col]))
        file.write("},\n")
    file.write("\t\t};\n")
    
    # Write end portion of the file.
    file.write(FILE_END)
    
    # Close finished file.
    file.close()

# Main Execution.
if __name__ == "__main__":
    # Serve help flag.
    if (len(sys.argv) == 2) and (sys.argv[1] == '-h'):
        print("python genScanTable.py [-h] <input file>")
        sys.exit(0)

    # Check file is provided.
    if (len(sys.argv) < 2) or not os.path.isfile(sys.argv[1]):
        print("Existing file not provided!")
        sys.exit(1)
    
    # Prepare to keep track of file information.
    subStates = {SCAN_START:0}
    endStates = {SCAN_ERROR:0}
    excStates = {}
    rules = {}
    
    # Parse file.
    lines = open(sys.argv[1], 'r').readlines()
    ruleNum = 0 # Allows rules to be read back in order
    for line in lines:
        # Chop off newline.
        line = line.replace('\n', '')
        
        # Ignore comments and spacers.
        if (len(line) == 0 or line[0] == '#'): continue
        
        # Ensure rule has 3 parts to use.
        tkns = line.split("::")
        if (len(tkns) != 3):
            print("Unrecognized format: %s"%line)
            sys.exit(1)
        
        # Record rule data.
        rules[ruleNum] = (tkns[0], tkns[1], tkns[2])
        ruleNum += 1
        if (re.fullmatch(r'[_a-z]+', tkns[0])): subStates[tkns[0]] = 0
        if (re.fullmatch(r'[_a-z]+', tkns[2])): subStates[tkns[2]] = 0
        if (re.fullmatch(r'[_A-Z]+', tkns[2])): endStates[tkns[2]] = 0
        if (re.fullmatch(r'[_A-Z]+\*', tkns[2])): excStates[tkns[2]] = 0
    
    # Consolidate states and assign index/enum values.
    allStates = {}
    num = 0
    for state in subStates:     # Ensure subStates < endStates < excStates
        allStates[state] = num
        num += 1
    if (num >= 100):            # Ensure 100-199 is unique to end/exc states
        print("Too many substates for scheme! Refactor design and/or generator!")
        sys.exit(1)
    allStates[SCAN_END_MIN] = 100
    num = 100
    for state in endStates:
        allStates[state] = num
        num += 1
    allStates[SCAN_EXCLUDE_MIN] = num
    for state in excStates:
        allStates[state] = num
        num += 1
    allStates[SCAN_END_MAX] = num - 1 # Set equal to last state
    if (num >= 200):            # Ensure 100-199 is unique to end/exc states
        print("Too many endstates for scheme! Refactor design and/or generator!")
        sys.exit(1)
    
    # Ensure solution is feasible (1-byte state preferred).
    if (len(allStates) > 256):
        print("Over 256 states- refactor design and/or generator!")
        sys.exit(1)

    # Create model of table (with SCAN_ERROR state as default).
    table = [[allStates[SCAN_ERROR] for col in range(LANG_NUM_CHARS)] for row in range(len(subStates))]
    
    # Populate model based on parsed rules.
    for numRule in rules:
        # Get base data.
        rowIdx = allStates[rules[numRule][0]]
        colRegex = rules[numRule][1]
        value = allStates[rules[numRule][2]]
        
        # Populate row based on regex.
        for colIdx in range(LANG_NUM_CHARS):
            if (re.fullmatch(colRegex, colIdxToChar(colIdx))):
                table[rowIdx][colIdx] = value
    
    # Generate files.
    genTableFile(table, allStates)