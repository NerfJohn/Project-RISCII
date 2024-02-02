'''
Scan Switch Generator:

ie "Quick solution to writing scenario-specific case statement"
'''

import re
import sys

# Details about switch's environment.
ENV_STATE_BASE = 33
ENV_SPACE_BASE = 12

# Cmd line argument pertinent infomation.
ARG_HELP_STRING  = "Usage: python genScanSwitch.py [-h] <rule file- .txt file>"
ARG_FLAG_PREFIX  = "-"
ARG_HELP_KEYWORD = "-h"

# Various parsing "magic numbers".
FILE_CASE_REGEX = r'[_a-zA-Z0-9]+:'
FILE_TOKEN_REGEX = r'[_A-Z0-9]+'
FILE_COMMENT_REGEX = r'(?://|#).*'

'''
Handles cmd line arguments passed to python script.

@return name of provided ruleset file
'''
def handleArgs():
    # No-arg call should be informative (albeit still be an error).
    if len(sys.argv) < 2:   # argv[0] = "genSyntax.py"
        print(ARG_HELP_STRING)
        sys.exit(1)
    
    # Parse arguments/options (except for argv[0] = "genSyntax.py").
    ruleFilename = None
    for i in range(1,len(sys.argv)):
        # Get argument.
        arg = sys.argv[i]
        
        # Parse flags (as applicable).
        if ARG_FLAG_PREFIX in arg:
            # Help Flag- print information and (successfully) exit.
            if ARG_HELP_KEYWORD == arg:
                print(ARG_HELP_STRING)
                sys.exit(0)
            
            # Bad flag- inform user and exit.
            print("Bad flag: \"%s\"- see \"python genSyntax.py -h\""%arg)
            sys.exit(1)
        
        # Parse non-flags as file- but only allow one.
        if ruleFilename != None:
            print("Files \"%s\" and \"%s\" given- " \
                  "see \"python genSyntax.py -h\""%(ruleFilename, arg))
            sys.exit(1)
        else:
            ruleFilename = arg
    
    # Arguments parsed- return parsed filename (if found).
    if ruleFilename == None:
        print("No ruleset file provided- see \"python genSyntax.py -h\"")
        sys.exit(1)
    return ruleFilename

'''
# TODO- Main Decription
'''
if __name__ == "__main__":
    # Parse cmd line arguments (and get file with ruleset).
    ruleFilename = handleArgs()
    
    # Open file for reading rules.
    ruleFile = None
    try:
        ruleFile = open(ruleFilename, 'r')
    except:
        # Error opening file- report and exit.
        print("Couldn't open file \"%s\""%ruleFilename)
        sys.exit(1)

    # Read/parse into case statement lines.
    print("Generating \"%s\" cases..."%ruleFilename)
    switchLines = []
    subStates = ["_SUB_ERROR", "_SUB_START"]
    for line in ruleFile.readlines():
        # Sanitize input line.
        line = line.replace('\n', '').replace('\r', '').strip()
        
        # Ignore comments/spacer lines.
        if len(line) == 0 or re.fullmatch(FILE_COMMENT_REGEX, line):
            continue
        
        # Break into component parts.
        line = line.replace(" ","\t").replace("\s"," ") # to get spaces
        tkns = [x for x in line.split("\t") if x]       # to get all tokens
        
        # Parse into case line.
        pLine = ""
        if re.fullmatch(FILE_CASE_REGEX, tkns[0]):
            # New case- ensure has a index number.
            state = tkns[0].replace(":", "")
            if not state in subStates:
                subStates.append(state)
            
            # Generate/save line:
            newState = state
            if not re.fullmatch(FILE_TOKEN_REGEX, newState):
                newState = subStates.index(newState) + ENV_STATE_BASE
            pLine += (' ' * ENV_SPACE_BASE) + "case %s:"%(newState)
            switchLines.append(pLine)
        elif tkns[0] == "IS":
            # Add sub-state if present.
            if not re.fullmatch(FILE_TOKEN_REGEX, tkns[2]) and \
               not tkns[2] in subStates:
               subStates.append(tkns[2])
            
            # Generate/save line.
            newState = tkns[2]
            if not re.fullmatch(FILE_TOKEN_REGEX, newState):
                newState = subStates.index(newState) + ENV_STATE_BASE
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "if (IS('%s')) {%s = %s; break;}" \
                %(tkns[1], "nextState", newState)
            switchLines.append(pLine)
        elif tkns[0] == "IN":
            # Add sub-state if present.
            if not re.fullmatch(FILE_TOKEN_REGEX, tkns[3]) and \
               not tkns[3] in subStates:
               subStates.append(tkns[3])
            
            # Generate/save line.
            newState = tkns[3]
            if not re.fullmatch(FILE_TOKEN_REGEX, newState):
                newState = subStates.index(newState) + ENV_STATE_BASE
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "if (IN('%s','%s')) {%s = %s; break;}" \
                %(tkns[1], tkns[2], "nextState", newState)
            switchLines.append(pLine)
        elif tkns[0] == "LBL":
            # Add sub-state if present.
            if not re.fullmatch(FILE_TOKEN_REGEX, tkns[1]) and \
               not tkns[1] in subStates:
               subStates.append(tkns[1])
            
            # Generate/save line.
            newState = tkns[1]
            if not re.fullmatch(FILE_TOKEN_REGEX, newState):
                newState = subStates.index(newState) + ENV_STATE_BASE
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "if (LBL) {%s = %s; break;}" \
                %("nextState", newState)
            switchLines.append(pLine)
        elif tkns[0] == "!LBL":
            # Add sub-state if present.
            if not re.fullmatch(FILE_TOKEN_REGEX, tkns[1]) and \
               not tkns[1] in subStates:
               subStates.append(tkns[1])
            
            # Generate/save line.
            newState = tkns[1]
            if not re.fullmatch(FILE_TOKEN_REGEX, newState):
                newState = subStates.index(newState) + ENV_STATE_BASE
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "if (!LBL) {%s = %s; break;}" \
                %("nextState", newState)
            switchLines.append(pLine)
        elif tkns[0] == "HEX":
            # Add sub-state if present.
            if not re.fullmatch(FILE_TOKEN_REGEX, tkns[1]) and \
               not tkns[1] in subStates:
               subStates.append(tkns[1])
            
            # Generate/save line.
            newState = tkns[1]
            if not re.fullmatch(FILE_TOKEN_REGEX, newState):
                newState = subStates.index(newState) + ENV_STATE_BASE
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "if (HEX) {%s = %s; break;}" \
                %("nextState", newState)
            switchLines.append(pLine)
        elif tkns[0] == "!HEX":
            # Add sub-state if present.
            if not re.fullmatch(FILE_TOKEN_REGEX, tkns[1]) and \
               not tkns[1] in subStates:
               subStates.append(tkns[1])
            
            # Generate/save line.
            newState = tkns[1]
            if not re.fullmatch(FILE_TOKEN_REGEX, newState):
                newState = subStates.index(newState) + ENV_STATE_BASE
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "if (!HEX) {%s = %s; break;}" \
                %("nextState", newState)
            switchLines.append(pLine)
        elif tkns[0] == "ELSE":
            # Add sub-state if present.
            if not re.fullmatch(FILE_TOKEN_REGEX, tkns[1]) and \
               not tkns[1] in subStates:
               subStates.append(tkns[1])
            
            # Generate/save line.
            newState = tkns[1]
            if not re.fullmatch(FILE_TOKEN_REGEX, newState):
                newState = subStates.index(newState) + ENV_STATE_BASE
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "%s = %s; break;" \
                %("nextState", newState)
            switchLines.append(pLine)
        elif tkns[0] == "EOF":
            # Add sub-state if present.
            if not re.fullmatch(FILE_TOKEN_REGEX, tkns[1]) and \
               not tkns[1] in subStates:
               subStates.append(tkns[1])
            
            # Generate/save line.
            newState = tkns[1]
            if not re.fullmatch(FILE_TOKEN_REGEX, newState):
                newState = subStates.index(newState) + ENV_STATE_BASE
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "if (IS(255)) {%s = %s; break;}" \
                %("nextState", newState)
            switchLines.append(pLine)
        else:
            print("Bad parse: \"%s\""%line)
            sys.exit(1)
    
    # Write to file.
    file = open(ruleFilename + ".out", "w")
    for line in switchLines:
        file.write(line + "\n")
    file.close()
    
    # Report final state count (for finishing off file details).
    print("Cases generated under \"%s\"- %d sub-states created"%(ruleFilename + ".out", len(subStates) - 1))