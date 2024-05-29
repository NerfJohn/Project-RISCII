'''
asBin.py: Script to write binary files quicker.

TODO- refactor for readability/extensibility

The intention of this script is to ease writing raw binary file for the RISCII
microprocessor (in the absence of the compiler- which has not been made at the
time of this writing). Format is "per line" and shown below.

Per line: [1s, 0s, or whitespace- 16 1s/0s exact] [Comment beginning with //]
    (Lines can can be just binary, just comments, neither, or both)
'''

from os import path as fs
import re
import sys

from pathlib import Path

################################################################################
## -- Constants/Globals --
################################################################################

# Definitions for return values from asBin.py
RET_GOOD          = 0
RET_FAIL          = 1

# Definitions for handling arguments.
ARGS_HELP_OPTION  = "-h"

# Definitions for expressions expected in given file.
EXPR_COMMENT      = "//"
EXPR_BIN_LINE     = "[01]{16}"

################################################################################
## -- Functions --
################################################################################

## -----------------------------------------------------------------------------
'''
Parses arguments for parser script. Returns filename.

@return filename
'''
def handleArgs():
    # Print usage for help request (or lack of arguments).
    if len(sys.argv) < 2 or sys.argv[1] == ARGS_HELP_OPTION:
        print("usage: asBin.py [-h] <file>")
        sys.exit(RET_FAIL if len(sys.argv) < 2 else RET_GOOD)
    
    # Extract/check filename.
    retFilename = sys.argv[1]
    if not fs.exists(retFilename) or fs.isdir(retFilename):
        print("\"%s\" is not a valid/existing file"%(retFilename))
        sys.exit(RET_FAIL)
    
    # Return parsed arguments.
    return retFilename

## -----------------------------------------------------------------------------
'''
Main program- conducts main procedure + progress reports of programming.
'''
if __name__ == "__main__":
    # Parse args.
    inFilename = handleArgs()

    # Open file for reading rules.
    ruleFile = None
    try:
        ruleFile = open(inFilename, 'r')
    except:
        # Error opening file- report and exit.
        print("Couldn't open file \"%s\""%inFilename)
        sys.exit(RET_FAIL)
    
    # Scan/Parse each line of user's file.
    lineNum = 0
    binBytes = bytes()
    for line in ruleFile.readlines():
        # Track line number (for reporting errors).
        lineNum = lineNum + 1
    
        # Sanitize input line (no \r\n markers, nor pure whitespace).
        line    = line.replace('\n', '').replace('\r', '').strip()
        binLine = line.replace('\t', '').replace(' ', '').strip()
        
        # Remove comments- focus on just binary numbers.
        cmtIdx = binLine.find(EXPR_COMMENT)
        if cmtIdx != -1:
            binLine = binLine[:cmtIdx]
        
        # Ignore spacer/blank lines.
        if len(binLine) == 0:
            continue
        
        # Ensure binary line is as expected.
        if not re.fullmatch(EXPR_BIN_LINE, binLine):
            print("Line %d: No 16 digit binary number '%s'"%(lineNum, line))
            sys.exit(RET_FAIL)
        
        # Convert/saved as bytes.
        binBytes = binBytes + int(binLine, 2).to_bytes(len(binLine) // 8, 'big')
    
    print(binBytes)
    
    #Path(inFilename + ".bin").write_bytes(binBytes)
    
    # Open file (of similar name) to put raw bytes into).
    outFilename = inFilename + ".bin"
    outFile = None
    try:
        outFile = open(outFilename, 'wb', encoding=None)
    except:
        # Error opening file- report and exit.
        print("Couldn't open file \"%s\""%outFilename)
        sys.exit(RET_FAIL)
    
    # Write bytes to file.
    outFile.write(binBytes)
    
    # Close writtn file.
    outFile.close()