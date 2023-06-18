# De-assembler: Helper function for debugging
# by John Eslinger

import os
import re
import sys

# Get instruction's name from its value
def instrValToName(value):
    match value:
        case 0: return "HLT"
        case 1: return "NOP"
        case 2: return "JAL"
        case 3: return "BRC"
        case 4: return "LDR"
        case 5: return "JPR"
        case 6: return "JLR"
        case 7: return "STR"
        case 8: return "ADD"
        case 9: return "SUB"
        case 10: return "LBI"
        case 11: return "XOR"
        case 12: return "SHL"
        case 13: return "SHR"
        case 14: return "ORR"
        case 15: return "AND"
        case -1: return "???" # default case

# Extracts value from given value.
def ex(val, off, msk):
    return (val >> off) & msk

# Main Execution.
if __name__ == "__main__":
    # Serve help flag.
    if (len(sys.argv) == 2) and (sys.argv[1] == '-h'):
        print("python undoAsm.py [-h] <input file>")
        sys.exit(0)

    # Check file is provided.
    if (len(sys.argv) < 2) or not os.path.isfile(sys.argv[1]):
        print("Existing file not provided!")
        sys.exit(1)
    
    # Parse file.
    lines = open(sys.argv[1], 'r').readlines()
    asmLines = {}
    asmLineNum = 0
    fullLine = ""
    for line in lines:
        # Chop off newline.
        line = line.replace('\n', '')
        
        # Append to get full 16-bit instruction.
        fullLine += line
        
        # Wait for 2nd byte to be appended before continuing to parsing.
        if len(fullLine) < 4: continue
        
        ### (Now processing one instruction) ###
        
        # Error out if line doesn't make sense.
        if (not re.fullmatch(r'[0-9a-f]{4}', fullLine)):
            print("Unrecognized format: %s"%fullLine)
            sys.exit(1)
        
        # Get numerical version of instruction.
        instrInt = int(fullLine, 16)
        
        # Get name of instruction type.
        instrName = instrValToName((instrInt >> 12) & 0xf)
        
        # Get (roughly) the bit fields of the instruction as values.
        instrFields = {}
        if instrName in {"AND", "ORR", "XOR", "SHL", "SHR", "ADD", "SUB"}:
            instrFields = {ex(instrInt,9,0x7),ex(instrInt,6,0x7),ex(instrInt,5,0x1),ex(instrInt,0,0x1f)}
        if instrName in {"LBI", "BRC", "JAL"}:
            instrFields = {ex(instrInt,9,0x7),ex(instrInt,0,0x1ff)}
        if instrName in {"LDR", "STR", "JPR", "JLR"}:
            instrFields = {ex(instrInt,9,0x7),ex(instrInt,6,0x7),ex(instrInt,0,0x3f)}
        if instrName in {"NOP", "HLT", "???"}:
            instrFields = {}
        
        # Add parsed line to record
        asmLines[asmLineNum] = fullLine + "   " + instrName
        for field in instrFields:
            asmLines[asmLineNum] += "   " + str(field)
        
        # Bookkeeping.
        fullLine = ""
        asmLineNum += 1
    
    # Write to file.
    file = open(sys.argv[1] + ".asm", "w")
    for line in asmLines:
        file.write(asmLines[line] + "\n");
    file.close()