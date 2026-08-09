################################################################################
# CliOpt.py: primary handler of cli arguments (driver/device/process layers).
# by John Eslinger
################################################################################

"""
CLI recognizes 3 types of tokens (in any order):
1) Lone Flag     (LONE)   single token beginning with '-'
2) File Argument (FILE)   single token NOT beginning with '-'
3) Option Flag   (OPTION) lone flag followed by file argument

Note: For brevity, this file contains info appropriate multiple layers (e.g.
driver, device, etc).
"""

################################################################################

# Common imports.

import sys
from enum import Enum

import old.DataModel as DataModel

################################################################################
# Domain Layer
################################################################################

################################################################################
# Types of CLI tokens.
class CliType(Enum):
    INVALID = 0
    LONE    = 1
    FILE    = 2
    OPTION  = 3

################################################################################
# Flags (lone and option) of the disassembler program.
class CliFlag(Enum):
    # Special/Sentinel values
    INVALID   = 0
    LONE_BASE = 100
    FOUT_BASE = 200

    # Lone flags.
    HELP     = 100
    
    # Option flags.
    FOUT     = 200

################################################################################
# Driver Layer
################################################################################

################################################################################
# Returns CLI arguments (in order of appearance). Does NOT include the exe call.
def driver_getArgs():
    return sys.argv[1:]

################################################################################
# Device Layer
################################################################################

################################################################################

# Prefix for identifying flags.
FLAG_PREFIX = '-'

################################################################################
# Interpret string into type/flag (Defaults to INVALID types for no match).
def device_asCliFlag(tkn: str):
    # Switch case of checks.
    if   "-h" == tkn: return CliType.LONE,   CliFlag.HELP
    elif "-o" == tkn: return CliType.OPTION, CliFlag.FOUT

    # Failure to match.
    return CliType.INVALID, CliFlag.INVALID

################################################################################
# Device class to iterate through tokens. Program specific but no error throws.
class CliOpt():
    # Private vars to track iteration.
    m_args  = []
    m_idx   = 0

    # Public vars describing current argument.
    m_type  = CliType.INVALID
    m_flag  = CliFlag.INVALID
    m_arg   = ""
    m_value = ""

    ############################################################################
    # Init instance- prepping to iterate over arguments.
    def __init__(self):
        # Prep internals.
        self.m_tkns     = driver_getArgs()
        self.m_nextIdx  = 0

        # Prep vars.
        self.m_type  = CliType.INVALID
        self.m_flag  = CliFlag.INVALID
        self.m_arg   = ""
        self.m_value = ""
    
    ############################################################################
    # Iterate through arguments. Returns true on success- false otherwise.
    def nextOpt(self):
        # Reset "outputs".
        self.m_type  = CliType.INVALID
        self.m_flag  = CliFlag.INVALID
        self.m_arg   = ""
        self.m_value = ""

        # Determine if required to parse.
        doIter = self.m_nextIdx < len(self.m_tkns)

        # Iterate as needed.
        if doIter:
            # Consume token.
            self.m_arg = self.m_tkns[self.m_nextIdx]
            self.m_nextIdx += 1

            # Determine type.
            if self.m_arg[0] == FLAG_PREFIX: 
                self.m_type, self.m_flag = device_asCliFlag(self.m_arg)
            else:
                self.m_type = CliType.FILE
                self.m_flag = CliFlag.INVALID
            
            # Consume option arg as needed.
            isOption = self.m_type is CliType.OPTION
            hasValue = self.m_nextIdx < len(self.m_tkns)
            if  isOption and hasValue:
                if self.m_tkns[self.m_nextIdx][0] != FLAG_PREFIX: # ensure short circuited check
                    self.m_value = self.m_tkns[self.m_nextIdx]
                    self.m_nextIdx += 1

        # Return if iteration was done.
        return doIter

################################################################################
# Process Layer
################################################################################

################################################################################
# Process input arguments into the data model.
def process_getCliArgs(model: DataModel.DataModel):
    # Get iterable arguments.
    args = CliOpt()

    # Per argument.
    while args.nextOpt():        
         # Hard fail for option flags without options.
        if (args.m_type == CliType.OPTION) and (0 >= len(args.m_value)):
            print(f"Option flag '{args.m_arg}' must be followed by a value")
            sys.exit()

        # Parse argument into model.
        if args.m_type == CliType.FILE:
            model.m_inFile = args.m_arg
        elif args.m_flag == CliFlag.HELP:
            print("RISCII Disassembler- convert bin to assembly")
            print("Usage: ./run.sh [opts] <file>")
            print("")
            print("Options:")
            print("    -h        show this menu and exit")
            print("    -o <file> set output file name")
            sys.exit()
        elif args.m_flag == CliFlag.FOUT:
            model.m_outFile = args.m_value
        
        # Hard fail on unknown arguments/flags.
        else:
            print(f"Unrecognized argument '{args.m_arg}'")
            sys.exit()
    
    # Require input file.
    if 0 >= len(model.m_inFile):
        print("No input file given")
        sys.exit()
    
    # Base output off of input file if not given.
    if 0 >= len(model.m_outFile):
        model.m_outFile = model.m_inFile + ".dasm.asm" # '.dasm' to mitigate source overwrites
