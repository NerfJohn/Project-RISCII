'''
Parse Switch Generator:

ie "Quick solution to writing scenario-specific case statement"
'''

import re
import sys

# Details about switch's environment.
ENV_STATE_BASE = 49
ENV_SPACE_BASE = 12

# Cmd line argument pertinent infomation.
ARG_HELP_STRING  = "Usage: python genParseSwitch.py [-h] <rule file- .txt file>"
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
    caseLines = []
    patLines = []
    subStates = ["_SUB_NULL", "_SUB_START"]
    patStates = {}
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
            caseLines.append(pLine)
        elif tkns[0] == "END":
            # Generate/save line.
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "nextIdx = -1; break;"
            caseLines.append(pLine)
        else:
            # Break into component parts.
            line = line.replace(" ","\t")
            tkns = [x for x in line.split("\t") if x]       # to get all tokens
            
            # Extract pertinent details
            cond = tkns[0]
            patId = "".join(tkns[1:])
            
            # Add pattern if not already added.
            if not patId in patStates:
                # Add pattern to array.
                idx = len(patLines)
                for i in range(len(tkns[1:])):
                    # Ensure sub-states are defined.
                    if not re.fullmatch(FILE_TOKEN_REGEX, tkns[i+1]) and \
                       not tkns[i+1] in subStates:
                        subStates.append(tkns[i+1])
                    patLines.append(tkns[i+1])
                patLines.append("_SUB_NULL")
                
                # Add key.
                patStates[patId] = idx
            
            # Generate/save line.
            newState = patStates[patId]
            pLine = (' ' * (ENV_SPACE_BASE + 4)) + "if (popTkn == %s) {nextIdx = %d; break;}" \
                %(cond, newState)
            caseLines.append(pLine)
              
    # Write to file (first switch, then pattern array).
    file = open(ruleFilename + ".out", "w")
    for line in caseLines:
        file.write(line + "\n")
    file.write("\n\nPattern Array (Below):\n\n")
    for item in patLines:
        value = item
        if value in subStates:
            value = subStates.index(value) + ENV_STATE_BASE
        file.write(str(value) + ",")
    file.close()
    
    # Report final state count (for finishing off file details).
    print("Cases generated under \"%s\"- %d sub-states and %d item array created" \
        %(ruleFilename + ".out", len(subStates) - 1, len(patLines)))