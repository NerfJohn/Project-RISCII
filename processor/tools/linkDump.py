'''
linkDump.py: Script to dump Project RISCII MCU contents.

TODO- refactor for readability/extensibility

'''

import math
from os import path as fs
from os import stat as fstat
import re
import serial
import sys
from time import sleep

from linkCmds import * # get access to "cmd line" commands

################################################################################
## -- Constants/Globals --
################################################################################

# Definitions for return values from linkDump.py
RET_GOOD          = 0
RET_FAIL          = 1

# Definitions for handling arguments.
ARGS_NUM_TYPICAL  = 3
ARGS_HELP_OPTION  = "-h"
ARGS_ADDR_REGEX   = '0x[a-fA-F0-9]{1,4}'
ARGS_LEN_REGEX    = '[0-9]+'
ARGS_COM_REGEX    = '[0-9]+'

# Definitions for reporting dumped data.
DUMP_ROW_LEN      = 8
DUMP_NUM_ADDRS    = 65536

################################################################################
## -- Functions --
################################################################################

## -----------------------------------------------------------------------------
'''
Parses arguments for programmer. Returns address, length, and com port.

@return start address, length of read, and com port
'''
def handleArgs():
    # Print usage for help request (or lack of arguments).
    if len(sys.argv) < ARGS_NUM_TYPICAL or sys.argv[1] == ARGS_HELP_OPTION:
        print("usage: linkDump.py [-h] <addr> <len> [com port number]")
        sys.exit(RET_FAIL if len(sys.argv) < ARGS_NUM_TYPICAL else RET_GOOD)
    
    # Extract/check start address.
    retAddr = sys.argv[1]
    if not re.fullmatch(ARGS_ADDR_REGEX, retAddr):
        print("\"%s\" is not a valid start address"%(retAddr))
        sys.exit(RET_FAIL)
    
    # Extract/check length.
    retLen = sys.argv[2]
    if not re.fullmatch(ARGS_LEN_REGEX, retLen):
        print("\"%s\" is not a valid length"%(retLen))
        sys.exit(RET_FAIL)
    
    # Attempt to get com port number
    retPort = None
    if len(sys.argv) > ARGS_NUM_TYPICAL:
        if re.fullmatch(ARGS_COM_REGEX, sys.argv[3]): retPort = int(sys.argv[3])
        else: print("warn: \"%s\" is not a valid com port"%(sys.argv[3]))
    
    # Return parsed arguments.
    return (retAddr, retLen, retPort)

## -----------------------------------------------------------------------------
'''
Main program- conducts main procedure + progress reports of dump.
'''
if __name__ == "__main__":
    # Parse args.
    address, dumpLen, comPort = handleArgs()
    
    # Update com port if specified.
    if comPort is not None:
        tkns = ["set_com", comPort]
        set_com(tkns)
    
    # Setup J-Link (and ensure it is connected).
    tkns = ["set_period", "0"]
    if set_period(tkns) is None: sys.exit(RET_FAIL)
    
    # Setup MCU for reading.
    tkns = ["jtag_pause"]
    jtag_pause(tkns)
    
    # Prep header.
    print(" Addr  |    +0     +1     +2     +3     +4     +5     +6     +7")
    print("-------+---------------------------------------------------------")
    
    # Read/print dumped data.
    numRows = math.ceil(int(dumpLen) / DUMP_ROW_LEN)
    numCols = DUMP_ROW_LEN
    for i in range(numRows):
        print("0x%s | "%(address[2:].zfill(4)), end="")
        for j in range(numCols):
            # Read data.
            tkns = ["mem_read", address]
            retBytes = mem_read(tkns)
            
            # Parse/save data.
            print(" %s"%("0x" + retBytes[1:3].hex()), end="")
            sys.stdout.flush()
            
            # Update address.
            address = hex((int(address, 16) + 1) % DUMP_NUM_ADDRS)
        print("")