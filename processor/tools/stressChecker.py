'''
stressChecker.py: Script to check core stress tests

Host script that interacts with RISCII processor running the core stress
test assembly program (ie ./tests/core/stress.txt). Waits until assembly
program pauses itself, checking the PC/register values are sensible per
iteration (or simply responding to pause asserted for testing purposes).

'''

import re

from linkCmds import * # get access to "cmd line" commands

################################################################################
## -- Constants/Globals --
################################################################################

# Definitions for return values from stressChecker.py
RET_GOOD          = 0
RET_FAIL          = 1

# Definitions for handling arguments.
ARGS_NUM_TYPICAL  = 1
ARGS_HELP_OPTION  = "-h"
ARGS_COM_REGEX    = '[0-9]+'

# Definitions of (HW) addresses for registers.
ADDR_MEM0         = "0x8000"
ADDR_MEM1         = "0x8001"
ADDR_MEM2         = "0x8002"
ADDR_CCTRL_PC     = "0xc002"
ADDR_CCTRL_SP     = "0xc003"
ADDR_TMR0_MAX     = "0xc012"
ADDR_TMR1_CTRL    = "0xc014"
ADDR_TMR1_MAX     = "0xc016"

# Definitions of (SW) PC values.
PC_TEST_CHECK     = "0x00D6"
PC_FINAL_HLT      = "0x00F6"

################################################################################
## -- Functions --
################################################################################

## -----------------------------------------------------------------------------
'''
Parses arguments for programmer. Returns com port.

@return com port
'''
def handleArgs():
    # Print usage for help request (or lack of arguments).
    if len(sys.argv) > ARGS_NUM_TYPICAL and sys.argv[1] == ARGS_HELP_OPTION:
        print("usage: stressChecker.py [-h] [com port number]")
        sys.exit(RET_GOOD)

    # Attempt to get com port number
    retPort = None
    if len(sys.argv) > ARGS_NUM_TYPICAL:
        if re.fullmatch(ARGS_COM_REGEX, sys.argv[1]): retPort = int(sys.argv[1])
        else: print("warn: \"%s\" is not a valid com port"%(sys.argv[1]))
    
    # Return parsed arguments.
    return retPort

## -----------------------------------------------------------------------------
'''
Helper function to command uP to undo HLT instruction's pause.
'''
def continueFromHLT():
    # Simply clear the register with HLT's pause bit.
    tkns = ["mem_write", "0xc000", "0x0000"]
    mem_write(tkns)

## -----------------------------------------------------------------------------
'''
Helper function to command uP to read a uP register.

@param hexStr register to read- presented as hex string with "0x" prefix
@return hex string with "0x" prefix of read value if successful, None otherwise

'''
def readReg(hexStr):
    # Read the uP register.
    tkns = ["mem_read", hexStr]
    retBytes = mem_read(tkns)
    
    # Report read data.
    if retBytes == None: return None
    else:                return "0x" + retBytes[1:3].hex().upper()

## -----------------------------------------------------------------------------
'''
Helper function to check uP registers are as expected after running test code.

@return array with max counts of both timers (arranged [TMR0, TMR1]) if good

'''
def checkRegisters():
    # Get the timer max counts.
    retCnts = [-1, -1]
    retCnts[0] = mem_read(["mem_read", ADDR_TMR0_MAX])[1:3].hex()
    retCnts[1] = mem_read(["mem_read", ADDR_TMR1_MAX])[1:3].hex()

    # Get the data points to check.
    sp = mem_read(["mem_read", ADDR_CCTRL_SP])[1:3].hex()
    m0 = mem_read(["mem_read", ADDR_MEM0])[1:3].hex()
    m1 = mem_read(["mem_read", ADDR_MEM1])[1:3].hex()
    m2 = mem_read(["mem_read", ADDR_MEM2])[1:3].hex()
    
    # Report the iteration (and stop) if if doesn't match up.
    if (sp != "0003") or (m0 != "0002") or (m1 != "0003") or (m2 != "0004"):
        # Not matching- report.
        print("ERROR! BAD MATCH")
        print("Last Maxes: TMR0 = %s, TMR = %s"%(retCnts[0], retCnts[1]))
        print("SP = %s, M0 = %s, M1 = %s, M2 = %s"%(sp, m0, m1, m2))
        
        # Stop testing.
        sys.exit(RET_FAIL)
    
    # All good! return max counts (ie iteration tested)
    return retCnts

## -----------------------------------------------------------------------------
'''
Main program- conducts main procedure + result of testing.
'''
if __name__ == "__main__":
    # Parse args.
    comPort = handleArgs()
    
    # Update com port if specified.
    if comPort is not None:
        tkns = ["set_com", comPort]
        set_com(tkns)
        
    # Setup J-Link (and ensure it is connected).
    tkns = ["set_period", "0"]
    if set_period(tkns) is None: sys.exit(RET_FAIL)
    print("Beginning test checking...")
    
    # Continuously check/report (broken from within).
    prevMaxes = [-1, -1]
    while True:
        # Wait for uP to pause.
        pc = readReg(ADDR_CCTRL_PC)
        while pc == None: pc = readReg(ADDR_CCTRL_PC)
        
        # Run appropriate procedure based on PC address detected.
        if pc < PC_TEST_CHECK:
            # Likely "pause" timer going off- turn off timer.
            tkns = ["mem_write", ADDR_TMR1_CTRL, "0x0000"]
            mem_write(tkns)
        elif pc == PC_TEST_CHECK:
            # Primary checkpoint reached- check uP for correctness.
            prevMaxes = checkRegisters();
            print("[%s, %s] passed"%(prevMaxes[0], prevMaxes[1]))
        elif pc == PC_FINAL_HLT:
            # Test program has finished running tests- exit.
            print("YAHOO! All tests pass!")
            sys.exit(RET_GOOD)
        else:
            # PC has somehow gone rouge- report what we know.
            print("ERROR! PC OUT OF RANGE");
            print("Last Maxes: TMR0 = %s, TMR = %s"%(prevMaxes[0], prevMaxes[1]))
            print("PC = %s"%(pc));
            sys.exit(RET_FAIL)
        sys.stdout.flush()
        
        # Continue with next test.
        continueFromHLT();
