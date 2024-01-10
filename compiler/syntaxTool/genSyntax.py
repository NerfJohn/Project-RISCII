'''
Syntax Generator: Converts syntax rules into C++ scanner/parser classes.

Uses input file with scanning/parsing rules (and preset templates) to implement
"state" classes to simplify transitioning/deciphering of file syntax. It is
intended to supplement scanning/parsing code, not completely replace it.

Syntax Rules
==============
1) Format: <cur_state>::<cond>::<new_state>
2) Sub-states = lower_case0, tokens = UPPER_CASE1, actions = #UPPER_CASE2
3) Scan Condition: [<chars>]- set of (peeked) chars
4) Parse Condition: EXISTING_TOKEN- implicily appended to new states
5) Scan new state: 1 non-action state, Parse new state: 0+ list of states
6) Rules applied in order of processing (ie line order)
7) Prepend "//" To make line a comment
==============
Built-ins:
"scan_start" : Starting Scan State, assumes empty buffer
"parse_start": Starting Parse State, assumes is lone state in stack
"EOF"        : End-of-File Token, common to both scanning/parsing
"ERROR"      : Error State, common to both scanning/parsing
'''

import re
import sys

# Cmd line argument pertinent infomation.
ARG_HELP_STRING  = "Usage: python genSyntax.py [-h] <rules file- .txt file>"
ARG_FLAG_PREFIX  = "-"
ARG_HELP_KEYWORD = "-h"

# File parsing pertinent information.
FILE_SYNTAX_DELIMITER = "::"
FILE_LIST_DELIMITER   = ","
FILE_NUM_SEGMENTS     = 3
FILE_COMMENT_REGEX    = r'(?://|#).*'
FILE_SUBSTATE_REGEX   = r'[_a-z0-9]+'
FILE_TOKEN_REGEX      = r'[_A-Z0-9]+'
FILE_ACTION_REGEX     = r'#[_A-Z0-9]+'
FILE_SCAN_COND_REGEX  = r'\[(?:\\\[|\\\]|[^\[\]])+\]'
FILE_PARSE_COND_REGEX = FILE_TOKEN_REGEX

# Keys for refering to segments of syntax rules.
SYNTAX_CUR_STATE = "SyntaxCurState"
SYNTAX_COND      = "SyntaxCond"
SYNTAX_NEW_STATE = "SyntaxNewState"

# Built-in states.
BUILTIN_SCAN_START  = "scan_start"
BUILTIN_PARSE_START = "parse_start"
BUILTIN_EOF         = "EOF"
BUILTIN_ERROR       = "ERROR"

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
Parses rule into its proper rulebook.

Rules are scanned/parsed. Assuming the rule is valid, it is either appended to
the scan rulebook or the parse rulebook.

@param rule string representing a rule
@param scanRulebook array of dictionaries with broken up/verified scan rules
@param parseRulebook array of dictionaries with broken up/verified parse rules
'''
def parseRule(rule, scanRulebook, parseRulebook):
    # Attempt to break up into segments.
    segments = rule.split(FILE_SYNTAX_DELIMITER)
    if len(segments) != FILE_NUM_SEGMENTS:
        print("Not enough segments in rule \"%s\""%rule)
        sys.exit(1)
    
    # Format segments into rulebook "entry".
    entry = {                                   \
        SYNTAX_CUR_STATE: segments[0].strip(),  \
        SYNTAX_COND:      segments[1].strip(),  \
        SYNTAX_NEW_STATE: segments[2].strip(),  \
    } 
    
    # Ensure current state is a sub-state.
    if not re.fullmatch(FILE_SUBSTATE_REGEX, entry[SYNTAX_CUR_STATE]):
        print("Current state is not a sub-state in rule \"%s\""%rule)
        sys.exit(1)
    
    # Determine new state parsing (by checking/parsing condition).
    if re.fullmatch(FILE_SCAN_COND_REGEX, entry[SYNTAX_COND]):
        # Scan rule- new state is one sub-state or token.
        if not re.fullmatch(FILE_SUBSTATE_REGEX, entry[SYNTAX_NEW_STATE]) and \
           not re.fullmatch(FILE_TOKEN_REGEX, entry[SYNTAX_NEW_STATE]):
            print("Invalid new state in rule \"%s\""%rule)
            sys.exit(1)

        # Valid scan rule- add to rulebook.
        scanRulebook.append(entry)
    elif re.fullmatch(FILE_PARSE_COND_REGEX, entry[SYNTAX_COND]):
        # Parse rule- new state is 0+ list of various state types.
        items = entry[SYNTAX_NEW_STATE].split(FILE_LIST_DELIMITER)
        for item in items:
            if not re.fullmatch(FILE_SUBSTATE_REGEX, item) and \
               not re.fullmatch(FILE_TOKEN_REGEX, item)    and \
               not re.fullmatch(FILE_ACTION_REGEX, item)   and \
               len(item) > 0:
                print("Invalid new state in rule \"%s\""%rule)
                sys.exit(1)
        
        # Valid parse rule- add to rulebook.
        parseRulebook.append(entry)
    else:
        print("Unrecognized condition in rule \"%s\""%rule)
        sys.exit(1)

'''
Sort scan sub-states and tokens into their arrays.

This function does NOT check/sort sub-states within the new state field
(following the idea they have been handled analyzing scan rules). If this
assumption is wrong, the syntax is broken as is.

@param rulebook parse rules to be analyzed
@param subnInfo array of scan sub-states to append to
@param tknInfo array of tokens to append to
'''
def analyzeScanRules(rulebook, subInfo, tknInfo):
    # Analyze each rule.
    for rule in rulebook:
        # Add current state if missing.
        if not rule[SYNTAX_CUR_STATE] in subInfo:
            subInfo.append(rule[SYNTAX_CUR_STATE])
        
        # Add new tokens as applicable.
        isTkn = re.fullmatch(FILE_TOKEN_REGEX, rule[SYNTAX_NEW_STATE])
        if isTkn and not rule[SYNTAX_NEW_STATE] in tknInfo:
            tknInfo.append(rule[SYNTAX_NEW_STATE])
        
    # (All sub-states should be referred to- can ignore for now.)

'''
Sort parse sub-states, patterns, and actions into their arrays.

This function does NOT check/sort tokens or sub-states within patterns
(following the idea they have been handled analyzing scan rules or other parse
rules). If this assumption is wrong, the syntax is broken as is.

@param rulebook parse rules to be analyzed
@param parseInfo array of parse sub-states to append to
@param newsInfo array of parse patterns to append to
@param actInfo array of actions to append to
'''
def analyzeParseRules(rulebook, parseInfo, newsInfo, actInfo):
    # Analyze each rule.
    for rule in rulebook:
        # Add current state if missing.
        if not rule[SYNTAX_CUR_STATE] in parseInfo:
            parseInfo.append(rule[SYNTAX_CUR_STATE])
        
        # Add parse pattern if missing.
        if not rule[SYNTAX_NEW_STATE] in newsInfo:
            newsInfo.append(rule[SYNTAX_NEW_STATE])
        
        # Check for new actions in pattern.
        items = items = rule[SYNTAX_NEW_STATE].split(FILE_LIST_DELIMITER)
        for item in items:
            isAct = re.fullmatch(FILE_ACTION_REGEX, item)
            if isAct and not item in actInfo:
                actInfo.append(item)
    
    # (All patterns' sub-states/tokens should be referred to- ignore for now.)

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
    
    # Read/parse each line into "rulebooks" for scanning and parsing.
    print("Reading \"%s\" rules..."%ruleFilename)
    scanRulebook  = []  # Array of dictionaries
    parseRulebook = []  # Array of dictionaries
    for line in ruleFile.readlines():
        # Sanitize input line.
        line = line.replace('\n', '').replace('\r', '').strip()
        
        # Ignore comments/spacer lines.
        if len(line) == 0 or re.fullmatch(FILE_COMMENT_REGEX, line):
            continue
        
        # Parse into correct rulebooks.
        parseRule(line, scanRulebook, parseRulebook)
        
    # Input info gathered- close the file.
    print("\t%d scan rules"%len(scanRulebook))
    print("\t%d parse rules"%len(parseRulebook))
    ruleFile.close()

    # Get preliminary indexing of states/patterns.
    print("Analyzing rules...")
    scanSubs   = [BUILTIN_SCAN_START]
    parseSubs  = [BUILTIN_PARSE_START]
    parseNews  = []
    allTokens  = [BUILTIN_ERROR, BUILTIN_EOF]
    allActions = []
    analyzeScanRules(scanRulebook, scanSubs, allTokens)
    analyzeParseRules(parseRulebook, parseSubs, parseNews, allActions)
    print("\t%d scan sub-states"%len(scanSubs))
    print("\t%d parse sub-states"%len(parseSubs))
    print("\t%d parse patterns"%len(parseNews))
    print("\t%d tokens"%len(allTokens))
    print("\t%d actions"%len(allActions))
    
    # TODO- amalgate stuff
    subMax = len(scanSubs)
    if subMax < len(parseSubs):
        subMax = len(parseSubs)
    print("Sub-state indices = %d"%subMax)
    print("Token indices = %d"%len(allTokens))
    print("Action indices = %d"%len(allActions))
    totIdx = subMax + len(allTokens) + len(allActions)
    print("Enum values needed = %d"%totIdx)
    