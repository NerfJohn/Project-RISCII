'''
linkPrgm.py: Script to program Project RISCII MCU over JTAG.

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

# Definitions for return values from linkPrgm.py
RET_GOOD          = 0
RET_FAIL          = 1

# Definitions for handling arguments.
ARGS_HELP_OPTION  = "-h"
ARGS_COM_REGEX    = '[0-9]+'

# Definitions for targetting MCU.
TARGET_MAX_SIZE   = 65536
TARGET_PAGE_SIZE  = 128
TARGET_WRITE_TOUT = 0.005

################################################################################
## -- Functions --
################################################################################

## -----------------------------------------------------------------------------
'''
Parses arguments for programmer. Returns filename and com port number.

@return filename and com port number
'''
def handleArgs():
    # Print usage for help request (or lack of arguments).
    if len(sys.argv) < 2 or sys.argv[1] == ARGS_HELP_OPTION:
        print("usage: linkPrgm.py [-h] <bin file> [com port number]")
        sys.exit(RET_FAIL if len(sys.argv) < 2 else RET_GOOD)
    
    # Extract/check filename.
    retFilename = sys.argv[1]
    if not fs.exists(retFilename) or fs.isdir(retFilename):
        print("\"%s\" is not a valid/existing file"%(retFilename))
        sys.exit(RET_FAIL)
    
    # Attempt to get com port number
    retPort = None
    if len(sys.argv) >= 3:
        if re.fullmatch(ARGS_COM_REGEX, sys.argv[2]): retPort = int(sys.argv[2])
        else: print("warn: \"%s\" is not a valid com port"%(sys.argv[2]))
    
    # Return parsed arguments.
    return (retFilename, retPort)

## -----------------------------------------------------------------------------
'''
Main program- conducts main procedure + progress reports of programming.
'''
if __name__ == "__main__":
    # Parse args.
    binFile, comPort = handleArgs()
    
    # Make sure binary will fit properly onto target.
    if fstat(binFile).st_size > TARGET_MAX_SIZE:
        print("file \"%s\" is too large (> %d bytes"%(binFile, TARGET_MAX_SIZE))
        sys.exit(RET_FAIL)
    
    # Update com port if specified.
    if comPort is not None:
        tkns = ["set_com", comPort]
        set_com(tkns)
    
    # Report progress.
    print("Programming \"%s\" to target..."%(binFile))
    sys.stdout.flush()
    
    # Setup J-Link (and ensure it is connected).
    tkns = ["set_period", "0"]
    if set_period(tkns, False) is None: sys.exit(RET_FAIL)
    
    # Setup MCU for programming.
    tkns = ["jtag_pause"]
    jtag_pause(tkns, False)
    
    # Open file to read/copy into MCU.
    binBytes = open(binFile, 'rb')
    
    # Report progress.
    print("Beginning programming...")
    sys.stdout.flush()
    
    # Write to MCU (in units of storage chip's pages).
    numPages = math.ceil(fstat(binFile).st_size / TARGET_PAGE_SIZE)
    for pageNum in range(numPages):
        # Get page worth of bytes to send.
        pageBytes = "0x"
        for byteNum in range(TARGET_PAGE_SIZE): # TODO- naturally stops at end?
            pageBytes = pageBytes + bytes(binBytes.read(1)).hex()
        
        # Write to MCU's storage chip.
        addr = hex(pageNum * TARGET_PAGE_SIZE)
        tkns = ["store_write", addr, pageBytes]
        store_write(tkns)
        
        # Report page writing to user.
        print("%d/%d pages written"%(pageNum + 1, numPages), end = '\r')
        sys.stdout.flush()
        
        # Wait for MCU to recover from store write timeout.
        sleep(TARGET_WRITE_TOUT)
    print("") # Insert newline to prevent overwriting progress message
    
    # Report final progress/success.
    print("Finished programming- target may now be reset")
        