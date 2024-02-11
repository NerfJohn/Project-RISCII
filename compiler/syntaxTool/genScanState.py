'''
genScanState.py

Script to speed-up/automate writing scan transition logic

Author: John Eslinger
'''

'''
== Summary ==

This tool generates "ScanState" class that encapsulates a defined language's
scanner state machine. A class instance can handle scan transition logic and
recognition while the application code handles what to do with the results.

Keep in mind this tool is effectively a fancy "copy/paste" tool for creating
a scan tool. It performs some checks for convenience, but not many. Likewise,
this tool is meant to ease logic writing- it is not a high performance tool.

== Environment Assumptions ==

It is assumed that a "SyntaxToken_e.h" has been defined with enum values (of
prefix "TOKEN_") to define the application's tokens (including the values
TOKEN_MAX_VALUE and TOKEN_INVALID).
 
It is also assumed the generated header file is included only once in the
application program- in the .cpp file handling with the application's "Scan
Phase" code. Including more than once may cause compilation errors.

== Input Ruleset Syntax ==

Ruleset syntax is similar to case statement logic (with less boilerplate). See
below for a summary of the syntax and their use:

    == Structural Syntax ==
    -> Label: Non-keyword/Non-token word made from "label chars" (a-zA-Z0-9_)
        ~ Predeclared label "start" acts as initial reset state of ScanState
        ~ Predefined label "ERROR" acts as terminal state for bad scan
        ~ New labels become sub-states within ScanState
    -> Token: Word made of label chars (a-zA-Z0-9_) and prefix "TOKEN_"
        ~ Defines terminal states of ScanState
        ~ Defined/controlled in application's "SyntaxToken_e.h" file
    -> State Declaration: Declares new section of rules for given state
        ~ Syntax: "<label>:"
        ~ Effectively defines starting edge of transition
    
    == Rule Syntax ==
    -> IS keyword: Used to match peeked char with a given char
        ~ Syntax: "IS   <char> <label or token>
    -> IN keyword: Used to match peeked char with range of chars
        ~ Syntax: "IN   <low char> <high char> <label or token>
    -> LBL_CHAR keyword: Used to match peeked char with label char (a-zA-Z0-9_)
        ~ Syntax: "LBL_CHAR   <label or token>
    -> HEX_CHAR keyword: Used to match peeked char with hex char (a-fA-F0-9)
        ~ Syntax: "HEX_CHAR   <label or token>
    
    == Additional Syntax ==
    -> \s: Stand-in for "space" char for rules (replaced with ' ' in output)
    -> Comment: User oriented information that does not affect output
        ~ Syntax: Line beginning with "//" or "#"
'''

import os.path as fs
import re
import sys

# Return codes for the program
RETURN_GOOD = 0
RETURN_BAD  = 1

# Cmd line argument information/constants.
ARG_HELP_STRING = "Usage: python genScanState.py [-h] <input rule file> <output directory>"
ARG_NO_ARGS_NUM = 1
ARG_REGEX_FLAG  = r'-.*'
ARG_REGEX_HELP  = r'-h'
ARG_REQ_ARG_NUM = 2
ARG_IDX_IN_FILE = 0
ARG_IDX_OUT_DIR = 1

# Pertinent values within key data structures of script.
DS_ENUM_ERROR    = "ERROR"
DS_ENUM_START    = "start"
DS_CASE_NODE_LEN = 1

# Pertinent values for scanning/parsing user file.
SCAN_REGEX_COMMENT = r'(?://|#).*'
SCAN_SPACE_ALIAS   = "\s"
SCAN_REGEX_NODE    = r'[_a-zA-Z0-9]+:'
SCAN_REGEX_TOKEN   = r'TOKEN_[_a-zA-Z0-9]+'
SCAN_REGEX_DEST    = r'[_a-zA-Z0-9]+'
SCAN_NODE_SUFFIX   = ":"

# Dictionary of valid keywords- values reveal keyword's # of required arguments.
KEYWORD_DICT = {"IS": 2, "IN": 3, "LBL_CHAR": 1, "HEX_CHAR": 1}

# Pertinent values for generating header file.
GEN_TEMPLATE_PATH    = "./ScanStateTemplate.h"
GEN_PRODUCT_NAME     = "ScanState.h"
GEN_ENUM_SIGNAL      = "@enumStates"
GEN_CASE_SIGNAL      = "@caseData"
GEN_TAB              = "    "
GEN_ENUM_LINE_TP     = "%s = TOKEN_MAX_VALUE + %d,\n"
GEN_ENUM_INC_INIT    = 1
GEN_CASE_NODE_TP     = "case %s:\n"
GEN_CASE_LINE_TP     = "if (%s) {nextState = %s; break;}\n"
GEN_CASE_END_TP      = "break;\n"
GEN_CASE_LINE_PREFIX = "else "
GEN_SUB_PREFIX       = "SCAN_SUB_"

'''
Determines transition for given keyword and arguments. Quick solution to hard problem.

This function simply exists to iterate and apply keyword specific actions. Ideally,
this functionality would be implemented a more maintainable manner. For now, this
is a decent solution.

TODO- refactor to allow for ease of new keywords

@param tkns tokens of the tranition
@return transition based on passed tokens
'''
def getTransition(tkns):
    # Convert into transition based on keyword used.
    if tkns[0] == "IS": 
        clause = "IS('%s')"%(tkns[1])
        if not re.fullmatch(SCAN_REGEX_TOKEN, tkns[2]): tkns[2] = GEN_SUB_PREFIX + tkns[2]
        return GEN_CASE_LINE_TP%(clause, tkns[2])
    if tkns[0] == "IN": 
        clause = "IN('%s','%s')"%(tkns[1], tkns[2])
        if not re.fullmatch(SCAN_REGEX_TOKEN, tkns[3]): tkns[3] = GEN_SUB_PREFIX + tkns[3]
        return GEN_CASE_LINE_TP%(clause, tkns[3])
    else: 
        if not re.fullmatch(SCAN_REGEX_TOKEN, tkns[1]): tkns[1] = GEN_SUB_PREFIX + tkns[1]
        return GEN_CASE_LINE_TP%(tkns[0], tkns[1])

'''
Handle arguments passed to script. Handles flags and returns required arguments.

Handles parsing of arguments from user. Flags are processed while arguments are
checked for validity prior to passing back to caller for further use.

@return input filename and output directory
'''
def handleArgs():
    # No-arg call should be informative (albeit still be an error).
    if len(sys.argv) == ARG_NO_ARGS_NUM: # argv[0] = "genScanState.py"
        print(ARG_HELP_STRING)
        sys.exit(RETURN_BAD)
    
    # Parse arguments/options (except for argv[0] = "genScanState.py").
    reqArgs = []
    for i in range(1,len(sys.argv)): # argv[1] and onwards...
        # Get argument.
        arg = sys.argv[i]
        
        # Parse flags (as applicable).
        if re.fullmatch(ARG_REGEX_FLAG, arg):
            # Help Flag- print information and (successfully) exit.
            if re.fullmatch(ARG_REGEX_HELP, arg):
                print(ARG_HELP_STRING)
                sys.exit(RETURN_GOOD)
            
            # Bad flag- inform user and exit.
            print("Bad flag: \"%s\"- see \"python genScanState.py -h\""%arg)
            sys.exit(RETURN_BAD)
        
        # Parse non-flags as required args.
        reqArgs.append(arg)
    
    # Check expected number of required args is accurate.
    if len(reqArgs) != ARG_REQ_ARG_NUM:
        print("Expected %d args, %d given"%(ARG_REQ_ARG_NUM, len(reqArgs)))
        sys.exit(RETURN_BAD)
    
    # Check each argument is of expected type (and exists).
    inFilename = reqArgs[ARG_IDX_IN_FILE]
    outDirname = reqArgs[ARG_IDX_OUT_DIR]
    if not fs.exists(inFilename) or fs.isdir(inFilename):
        print("\"%s\" is not a valid/existing file"%(inFilename))
        sys.exit(RETURN_BAD)
    if not fs.exists(outDirname) or not fs.isdir(outDirname):
        print("\"%s\" is not a valid/existing directory"%(outDirname))
        sys.exit(RETURN_BAD)

    # Arguments parsed- return arguments for further use.
    return (inFilename, outDirname)

'''
Opens and scans given file into data structures. Reports errors in syntax as encountered.

Opens the given file and reads each line- parsing them based on the tool's limited
syntax. Performs minimal checks to 1) ensure data populated in data structures isn't
faulty and 2) to avoid discrete bugs in user code. Modifies provided data structures.

@param inFilename file to open/scan/parse
@param enumStates data structure with internal states inferred in the given file
@param caseData data structure with summary of cruicial pieces of transition logic
'''
def scanRules(inFilename, enumStates, caseData):
    # Open file for reading rules.
    ruleFile = None
    try:
        ruleFile = open(inFilename, 'r')
    except:
        # Error opening file- report and exit.
        print("Couldn't open file \"%s\""%inFilename)
        sys.exit(RETURN_BAD)
    
    # Scan/Parse each line of user's file.
    lineNum = 0
    inCase = False
    for line in ruleFile.readlines():
        # Next line.
        lineNum = lineNum + 1
    
        # Sanitize input line (no \n, \r, or padding whitespace).
        line = line.replace('\n', '').replace('\r', '').strip()
        
        # Ignore comments/spacer lines.
        if len(line) == 0 or re.fullmatch(SCAN_REGEX_COMMENT, line):
            continue
        
        # Break into component parts (aliased spaces resolved here).
        line = line.replace(" ","\t").replace(SCAN_SPACE_ALIAS," ") # resolve spaces
        tkns = [x for x in line.split("\t") if x]                   # tokenize by tabs
        
        # Parse the rule (either a node/state or edge/transition).
        firstTkn = tkns[0] # Previous checks guarantee at least one token
        if re.fullmatch(SCAN_REGEX_NODE, firstTkn):
            # Get the node's proper name.
            nodeName = firstTkn.replace(SCAN_NODE_SUFFIX, "")
            
            # Terminal tokens and "ERROR" cannot act as sub-states/nodes.
            if re.fullmatch(SCAN_REGEX_TOKEN, nodeName) or re.fullmatch(DS_ENUM_ERROR, nodeName):
                print("ERR  %s:%d  Node/State cannot be a \"TOKEN_\" name or ERROR"%(inFilename, lineNum))
                sys.exit(RETURN_BAD)
            
            # Otherwise, add to valid case/transition data.
            caseData.append([nodeName])
            
            # If the node/state is new, add it to the enum records.
            if not nodeName in enumStates:
                enumStates.append(nodeName)
            
            # Bookkeeping.
            inCase = True
        else:
            # Semantic check: Node/state declared before transitions.
            if not inCase:
                print("ERR  %s:%d  Must declare state before transition"%(inFilename, lineNum))
                sys.exit(RETURN_BAD)
        
            # As a edge/transition, ensure first token is a valid keyword.
            if KEYWORD_DICT[firstTkn] == None:
                print("ERR  %s:%d  Unknown keyword \"%s\""%(inFilename, lineNum, firstTkn))
                sys.exit(RETURN_BAD)
            
            # Ensure correct number of arguments were provided (based off keyword).
            numArgs = KEYWORD_DICT[firstTkn]
            if (len(tkns) - 1) != numArgs:  # -1 to account for keyword itself
                print("ERR  %s:%d %s expects %d args, %d given"%(inFilename, lineNum, firstTkn, numArgs, len(tkns) - 1))
                sys.exit(RETURN_BAD)
            
            # Finally, ensure destination is valid (can be terminal or non-terminal).
            destName = tkns[-1] # Last argument in tokens list
            if not re.fullmatch(SCAN_REGEX_DEST, destName):
                print("ERR  %s:%d \"%s\" is not a valid destination state"%(inFilename, lineNum, destName))
                sys.exit(RETURN_BAD)
            
            # All tests clear- add the valid case/transition data.
            caseData.append(tkns)
            
            # Add destination to enum records if new (and NOT a token).
            if not re.fullmatch(SCAN_REGEX_TOKEN, destName) and not destName in enumStates:
                enumStates.append(destName)
    
    # Sanity Check: At least some functional lines were given.
    if len(caseData) == 0:
        print("ERR  %s:%d No states/transitions declared"%(inFilename, lineNum))
        sys.exit(RETURN_BAD)

'''
Writes header file to given filename, copying or generating text into the new file.

Opens both the template file and new file, copying the contents of the template
until a signal is hit. Depending on the signal, the data structures will be used
to generate code. Resulting file should be C++ compile-ready.

@param outFilename file to write template/generated code to
@param enumStates data used to generate enum code
@param caseData data used to generated case statement logic
'''
def genHeader(outFilename, enumStates, caseData):
    # Open template for copying constant pieces.
    tplateFile = None
    try:
        tplateFile = open(GEN_TEMPLATE_PATH, 'r')
    except:
        # Error opening file- report and exit.
        print("Couldn't open template file \"%s\""%GEN_TEMPLATE_PATH)
        sys.exit(RETURN_BAD)
    
    # Open/create new file.
    genFile = None
    try:
        genFile = open(outFilename, 'w')
    except:
        # Error opening file- report and exit.
        print("Couldn't open/create generated file \"%s\""%outFilename)
        sys.exit(RETURN_BAD)
    
    # Read each template line into the new file- replacing as applicable.
    for line in tplateFile.readlines():
        # Handle generation of enum for nodes/states.
        if GEN_ENUM_SIGNAL in line:
            # Generate enum value for each internal state.
            incVal = GEN_ENUM_INC_INIT
            for name in enumStates:
                # Create.
                genLine = GEN_TAB + GEN_ENUM_LINE_TP%(GEN_SUB_PREFIX + name, incVal)
                
                # Write.
                genFile.write(genLine)
                
                # And prep for next.
                incVal = incVal + 1
        
        #Handle generation of cases for edges/transition logic.
        elif GEN_CASE_SIGNAL in line:
            # Generate case statement logic for each transition/state.
            hasCase = False # Minor details that control syntax
            hasIf   = False
            for tkns in caseData:
                # Line to generate/write.
                genLine = ""
            
                # Handle start of case statements.
                if len(tkns) == DS_CASE_NODE_LEN:
                    # Create line.
                    if hasCase:
                        genLine = GEN_TAB*3 + GEN_CASE_END_TP
                    genLine += GEN_TAB*2 + GEN_CASE_NODE_TP%(GEN_SUB_PREFIX + tkns[0])
                    
                    # Bookkeeping.
                    hasCase = True
                    hasIf = False
                
                # Otherwise, parse transition in case statement.
                else:
                    # Determine transition.
                    baseTransition = getTransition(tkns)
                    
                    # Create line.
                    if hasIf:
                        genLine = GEN_TAB*3 + GEN_CASE_LINE_PREFIX + baseTransition
                    else:
                        genLine = GEN_TAB*3 + baseTransition
                    
                    # Bookkeeping.
                    hasIf = True
                
                # Write to new file.
                genFile.write(genLine)
            
            # Ensure last case statement ends with break.
            genFile.write(GEN_TAB*3 + GEN_CASE_END_TP)
            
                
        # Otherwise, just copy the line.
        else:
            genFile.write(line) 

'''
Main Script Program: Translate user's rules into C++ ScanState header file.

Uses cmd line arguments to be given scan rules to turn into a C++ header
file encapsulating the ruleset (placed in the given directory). Provides
stdout information for process visibility and metrics.
'''
if __name__ == "__main__":
    # Parse cmd line arguments (and get file with ruleset).
    inFilename, outDirname = handleArgs()
    
    # Prepare main data structures for scanning/generating.
    enumStates = []  # Numbered list of internal states needed for translation
    caseData   = []  # List of lists with case statement/transition logic info
    enumStates.append(DS_ENUM_ERROR)  # Built-in states of ScanState 
    enumStates.append(DS_ENUM_START)
    
    # Read the user's rules into the data structures.
    print("Scanning/Parsing \"%s\" ruleset..."%(inFilename))
    scanRules(inFilename, enumStates, caseData)
    
    # Generate header file in requested directory.
    outFilename = fs.join(outDirname, GEN_PRODUCT_NAME)
    print("Generating \"%s\"..."%(outFilename))
    genHeader(outFilename, enumStates, caseData)
    
    # Compute/report relevant data for user.
    numStates = len(enumStates)
    numEdges = len([x for x in caseData if len(x) > 1])
    print("%s created (%d Internal states, %d Edges)"%(outFilename, numStates, numEdges))
