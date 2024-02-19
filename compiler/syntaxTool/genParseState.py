'''
genParsetate.py

Script to speed-up/automate writing parsing transition logic

Author: John Eslinger
'''

'''
== Summary ==

This tool generates "ParseState" class that encapsulates a defined language's
parser state machine. A class instance can handle parse transition logic and
recognition while the application code handles what to do with the results.

Keep in mind this tool is effectively a fancy "copy/paste" tool for creating
a parse tool. It performs some checks for convenience, but not many. Likewise,
this tool is meant to ease logic writing- it is not a high performance tool.

== Environment Assumptions ==

It is assumed that a "SyntaxToken_e.h" has been defined with enum values (of
prefix "TOKEN_") to define the application's tokens (including the values
TOKEN_MAX_VALUE and TOKEN_INVALID).

Similarly, it is assumed that a "ParseAction_e.h" has been defined with enum
values (of prefix "ACTION_") to define the application's tokens (including the
values ACTION_MAX_VALUE and ACTION_INVALID).
 
It is also assumed the generated header file is included only once in the
application program- in the .cpp file handling with the application's "Parse
Phase" code. Including more than once may cause compilation errors.

== Input Ruleset Syntax ==

Ruleset syntax is similar to case statement logic (with less boilerplate and
pushed pattern more obvious). See below for a summary of the syntax and their
use:

    == Structural Syntax ==
    -> Label: Non-keyword/Non-token word made from "label chars" (a-zA-Z0-9_)
        ~ Predeclared label "start" acts as initial reset state of ParseState
        ~ Predefined label "NULL" is an internal state- NOT for application use
        ~ New labels become sub-states within ParseState
    -> Token: Word made of label chars (a-zA-Z0-9_) and prefix "TOKEN_"
        ~ Defines "physical" states of ParseState
        ~ Defined/controlled in application's "SyntaxToken_e.h" file
    -> Action: Word made of label chars (a-zA-Z0-9_) and prefix "ACTION_"
        ~ Defines "when/what to do" scenarios of application using ParseState
        ~ Defined/controlled in application's "ParseAction_e.h" file
    -> State Declaration: Declares new section of rules for given state
        ~ Syntax: "<label>:"
        ~ Effectively defines starting edge of transition
    
    == Additional Syntax ==
    -> Typical Transition: <token> <0+ list of tokens, actions, states>
        ~ Each state must transition based off token (DFA design)
        ~ Tokens pushed right-to-left (ie so handled/parsed left-to-right)
        ~ Token driving transition implictly "included", then immediately popped
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
ARG_HELP_STRING = "Usage: python genParseState.py [-h] <input rule file> <output directory>"
ARG_NO_ARGS_NUM = 1
ARG_REGEX_FLAG  = r'-.*'
ARG_REGEX_HELP  = r'-h'
ARG_REQ_ARG_NUM = 2
ARG_IDX_IN_FILE = 0
ARG_IDX_OUT_DIR = 1

# Pertinent values within key data structures of script.
DS_ENUM_NULL    = "NULL"
DS_ENUM_START    = "start"
DS_CASE_NODE_LEN = 1

# Pertinent values for scanning/parsing user file.
SCAN_REGEX_COMMENT = r'(?://|#).*'
SCAN_REGEX_NODE    = r'[_a-zA-Z0-9]+:'
SCAN_REGEX_TOKEN   = r'TOKEN_[_a-zA-Z0-9]+'
SCAN_REGEX_ACTION  = r'ACTION_[_a-zA-Z0-9]+'
SCAN_REGEX_ITEM    = r'[_a-zA-Z0-9]+'
SCAN_NODE_SUFFIX   = ":"

# Pertinent values for generating header file.
GEN_TEMPLATE_PATH    = "./ParseStateTemplate.h"
GEN_PRODUCT_NAME     = "ParseState.h"
GEN_ENUM_SIGNAL      = "@enumStates"
GEN_LIST_SIGNAL      = "@patternList"
GEN_CASE_SIGNAL      = "@caseData"
GEN_TAB              = "    "
GEN_ENUM_LINE_TP     = "%s = ACTION_MAX_VALUE + %d,\n"
GEN_ENUM_INC_INIT    = 1
GEN_CASE_NODE_TP     = "case %s\n"
GEN_CASE_LINE_TP     = "if (%s == popTkn) {nextIdx = %d; break;}\n"
GEN_CASE_END_TP      = "break;\n"
GEN_CASE_LINE_PREFIX = "else "
GEN_CASE_COND_IDX    = 0
GEN_CASE_IDX_IDX     = 1
GEN_SUB_PREFIX       = "PARSE_SUB_"
GEN_LIST_MAX_COLS    = 80

'''
Handle arguments passed to script. Handles flags and returns required arguments.

Handles parsing of arguments from user. Flags are processed while arguments are
checked for validity prior to passing back to caller for further use.

@return input filename and output directory
'''
def handleArgs():
    # No-arg call should be informative (albeit still be an error).
    if len(sys.argv) == ARG_NO_ARGS_NUM: # argv[0] = "genParseState.py"
        print(ARG_HELP_STRING)
        sys.exit(RETURN_BAD)
    
    # Parse arguments/options (except for argv[0] = "genParseState.py").
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
            print("Bad flag: \"%s\"- see \"python genParseState.py -h\""%arg)
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
@param patternList data structure with patterns appended as one list
@param caseData data structure with summary of cruicial pieces of transition logic
'''
def scanRules(inFilename, enumStates, patternList, caseData):
    # Open file for reading rules.
    ruleFile = None
    try:
        ruleFile = open(inFilename, 'r')
    except:
        # Error opening file- report and exit.
        print("Couldn't open file \"%s\""%inFilename)
        sys.exit(RETURN_BAD)
    
    # Scan/Parse each line of user's file.
    idxDict = {} # Pair patterns to indices
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
        
        # Break into component parts.
        line = line.replace(" ","\t")                # unify "whitespaces" into tabs
        tkns = [x for x in line.split("\t") if x]    # tokenize by tabs
        
        # Parse the rule (either a node/state or edge/transition).
        firstTkn = tkns[0] # Previous checks guarantee at least one token
        if re.fullmatch(SCAN_REGEX_NODE, firstTkn):
            # Get the node's proper name.
            nodeName = firstTkn.replace(SCAN_NODE_SUFFIX, "")
            
            # Terminal tokens and "NULL" cannot act as sub-states/nodes.
            if re.fullmatch(SCAN_REGEX_TOKEN, nodeName) or \
               re.fullmatch(SCAN_REGEX_ACTION, nodeName) or \
               re.fullmatch(DS_ENUM_NULL, nodeName):
                print("ERR  %s:%d  Node/State cannot be a \"TOKEN_\"/\"ACTION_\" name or NULL"%(inFilename, lineNum))
                sys.exit(RETURN_BAD)
            
            # Otherwise, add to valid case/transition data
            caseData.append([firstTkn]) # save raw token to differ from case lines
            
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
        
            # As a edge/transition, ensure first token is a syntax token.
            if not re.fullmatch(SCAN_REGEX_TOKEN, firstTkn):
                print("ERR  %s:%d Rules must begin with a syntax token"%(inFilename, lineNum))
                sys.exit(RETURN_BAD)
                
            # Ensure all tokens are valid (TOKEN_, ACTION_, or internal label).
            for tkn in tkns:
                if not re.fullmatch(SCAN_REGEX_ITEM, tkn):
                    print("ERR  %s:%d %s is not a syntax token, action, or valid state name"%(inFilename, lineNum, tkn))
                    sys.exit(RETURN_BAD)
            
            # - All tests clear- add/record data - #
            
            # Add to pattern list (as necessary).
            patternId = "".join(tkns[1:])
            if not patternId in idxDict:
                # Update local dict.
                newIdx = len(patternList)
                idxDict[patternId] = newIdx
                
                # Update list.
                for token in (tkns[1:])[::-1]: # reverse push- states now handled left-to-right
                    patternList.append(token)
                patternList.append(GEN_SUB_PREFIX + DS_ENUM_NULL)
            
            # Add the valid case/transition data.
            caseData.append([firstTkn, idxDict[patternId]])
            
            # Add new sub-states to records.
            for tkn in tkns:
                if not re.fullmatch(SCAN_REGEX_TOKEN, tkn) and \
                   not re.fullmatch(SCAN_REGEX_ACTION, tkn) and \
                   not tkn in enumStates:
                    enumStates.append(tkn)
    
    # Sanity Check: At least some functional lines were given.
    if len(caseData) == 0:
        print("ERR  %s:%d No states/transitions declared"%(inFilename, lineNum))
        sys.exit(RETURN_BAD)
    
    # Sanity Check: "start" state defined.
    if [DS_ENUM_START + SCAN_NODE_SUFFIX] not in caseData:
        print("ERR  %s:%d \"start\" state not defined"%(inFilename, lineNum))
        sys.exit(RETURN_BAD)

'''
Writes header file to given filename, copying or generating text into the new file.

Opens both the template file and new file, copying the contents of the template
until a signal is hit. Depending on the signal, the data structures will be used
to generate code. Resulting file should be C++ compile-ready.

@param outFilename file to write template/generated code to
@param enumStates data used to generate enum code
@param patternList data structure with patterns appended as one list
@param caseData data used to generated case statement logic
'''
def genHeader(outFilename, enumStates, patternList, caseData):
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
        
        # Handle generation of pattern list as one block.
        elif GEN_LIST_SIGNAL in line:
            # Print list- respecting column count requirement.
            genLine = GEN_TAB
            for item in patternList:
                # Determine next item to add.
                nextItem = ""
                if item in enumStates:
                    nextItem = " " + GEN_SUB_PREFIX + item + ","
                else:
                    nextItem = " " + item + ","
                
                # Write line once full (per column requirement).
                if len(nextItem) + len(genLine) > GEN_LIST_MAX_COLS:
                    # Write to file.
                    genFile.write(genLine + "\n")
                    
                    # Reset generated line.
                    genLine = GEN_TAB
                
                # Append next item to generated line.
                genLine += nextItem
            
            # Ensure last generated line is written.
            genFile.write(genLine + "\n")
                
        
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
                    baseTransition = GEN_CASE_LINE_TP%(tkns[GEN_CASE_COND_IDX], tkns[GEN_CASE_IDX_IDX])
                    
                    # Create line. # TODO- fix hard-coded check.
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
Main Script Program: Translate user's rules into C++ ParseState header file.

Uses cmd line arguments to be given parse rules to turn into a C++ header
file encapsulating the ruleset (placed in the given directory). Provides
stdout information for process visibility and metrics.
'''
if __name__ == "__main__":
    # Parse cmd line arguments (and get file with ruleset).
    inFilename, outDirname = handleArgs()
    
    # Prepare main data structures for scanning/generating.
    enumStates  = []  # Numbered list of internal states needed for translation
    patternList = []  # Contiguous block of all possible patterns
    caseData    = []  # List of lists with case statement/transition logic info
    enumStates.append(DS_ENUM_NULL)  # Built-in states of ParseState 
    enumStates.append(DS_ENUM_START)
    
    # Read the user's rules into the data structures.
    print("Scanning/Parsing \"%s\" ruleset..."%(inFilename))
    scanRules(inFilename, enumStates, patternList, caseData)
    
    # Generate header file in requested directory.
    outFilename = fs.join(outDirname, GEN_PRODUCT_NAME)
    print("Generating \"%s\"..."%(outFilename))
    genHeader(outFilename, enumStates, patternList, caseData)
    
    # Compute/report relevant data for user.
    numStates = len(enumStates)
    numEdges = len([x for x in caseData if len(x) > 1])
    lenList = len(patternList)
    print("%s created (%d Internal states, %d Edges, %d slot pattern list)"%(outFilename, numStates, numEdges, lenList))
