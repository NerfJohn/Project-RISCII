# Quick Assembler: For use for testing hardware
# by John Eslinger

import sys
import os
import pathlib
import re

# Generates 'bits'-bit binary numbers from 'num'.
def imm(num, bits):
    binNum = bin(int(num)%2**bits).replace("0b", "")
    return binNum.zfill(bits)

# Generates binary register number.
def reg(num):
    return imm(num, 3)

# Generates binary number based on flags.
def flg(flags):
    if (flags == '...'): return imm(0, 3)
    elif (flags == '..p'): return imm(1, 3)
    elif (flags == '.z.'): return imm(2, 3)
    elif (flags == '.zp'): return imm(3, 3)
    elif (flags == 'n..'): return imm(4, 3)
    elif (flags == 'n.p'): return imm(5, 3)
    elif (flags == 'nz.'): return imm(6, 3)
    elif (flags == 'nzp'): return imm(7, 3)
    elif (flags == 's'): return '1'
    else:
        print("Unknown flag: %s"%flags)
        sys.exit(1)

# Generates binary number based on opcode.
def op(opcode):
    if (opcode == "AND"): return '1111'
    elif (opcode == "ORR"): return '1110'
    elif (opcode == "XOR"): return '1011'
    elif (opcode == "SHL"): return '1100'
    elif (opcode == "SHR"): return '1101'
    elif (opcode == "ADD"): return '1000'
    elif (opcode == "SUB"): return '1001'
    elif (opcode == "LBI"): return '1010'
    elif (opcode == "LDR"): return '0100'
    elif (opcode == "STR"): return '0111'
    elif (opcode == "BRC"): return '0011'
    elif (opcode == "JPR"): return '0101'
    elif (opcode == "JAL"): return '0010'
    elif (opcode == "JLR"): return '0110'
    elif (opcode == "NOP"): return '0001'
    elif (opcode == "HLT"): return '0000'
    else:
        print("Unknown opcode: %s"%opcode)
        sys.exit(1)

# Converts binary string to hex string.
def toHexWord(num):
    hexNum = hex(int(num,2)%65536).replace("0x", "")
    return hexNum.zfill(4)

# Main Execution.
if __name__ == "__main__":
    # Serve help flag.
    if (len(sys.argv) < 2) and (sys.argv == "-h"):
        print("python quickAsm.py [-h] <input .asm file>")
        sys.exit(1)

    # Check file is provided.
    if (len(sys.argv) < 2) or not os.path.isfile(sys.argv[1]) or (pathlib.Path(sys.argv[1]).suffix != ".asm"):
        print("Existing .asm file not provided!")
        sys.exit(1)
    
    # Parse each line of the file into an array.
    binArray = []
    lines = open(sys.argv[1], 'r').readlines()
    for line in lines:
        # Chop off newline.
        line = line.replace('\n','')
        
        # Match the string (brute force- simple and decently effective).
        if (re.search(r';.*', line)): continue
        elif (re.search(r'(NOP|HLT)', line)): binArray.append(op(line[0:3]) + '000000000000')
        elif (re.search(r'BRC -[nzp\.]* -?[0-9]+', line)): binArray.append(op(line[0:3]) + flg(line[5:8]) + imm(line[9:], 9))
        elif (re.search(r'LBI r[0-7] -?[0-9]+', line)): binArray.append(op(line[0:3]) + reg(line[5]) + '0' + imm(line[7:], 8))
        elif (re.search(r'LBI r[0-7] -s -?[0-9]+', line)): binArray.append(op(line[0:3]) + reg(line[5]) + '1' + imm(line[9:], 8))
        elif (re.search(r'JAL r[0-7] -?[0-9]+', line)): binArray.append(op(line[0:3]) + reg(line[5]) + imm(line[7:], 9))
        elif (re.search(r'JPR r[0-7] -?[0-9]+', line)): binArray.append(op(line[0:3]) + '000' + reg(line[5]) + imm(line[7:], 6))
        elif (re.search(r'LDR r[0-7] r[0-7] -?[0-9]+', line)): binArray.append(op(line[0:3]) + reg(line[5]) + reg(line[8]) + imm(line[10:], 6))
        elif (re.search(r'STR r[0-7] r[0-7] -?[0-9]+', line)): binArray.append(op(line[0:3]) + reg(line[5]) + reg(line[8]) + imm(line[10:], 6))
        elif (re.search(r'JLR r[0-7] r[0-7] -?[0-9]+', line)): binArray.append(op(line[0:3]) + reg(line[5]) + reg(line[8]) + imm(line[10:], 6))
        # (Handle pure data operations)
        elif (re.search(r'[A-Z]{3} r[0-7] r[0-7] r[0-7]', line)): binArray.append(op(line[0:3])+ reg(line[5]) + reg(line[8]) + '000' + reg(line[11]))
        elif (re.search(r'[A-Z]{3} r[0-7] r[0-7] -?[0-9]+', line)): binArray.append(op(line[0:3]) + reg(line[5]) + reg(line[8]) + '1' + imm(line[10:], 5))
        else:
            print("Unrecognized format: %s"%line)
            sys.exit(1)
    
    # Write result to file.
    outFile = open(pathlib.Path(sys.argv[1]).with_suffix('.hex'), 'w')
    for entry in binArray:
        # Write the hexified bytes to the output file.
        hexNum = toHexWord(entry)
        outFile.write(hexNum[0:2] + '\n')
        outFile.write(hexNum[2:] + '\n')
    outFile.close()
    
    # Exit successfully.
    sys.exit(0)
        