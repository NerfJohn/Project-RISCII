################################################################################
# CliOpt.py: cli argument parser
################################################################################

from enum import Enum
from typing import List

################################################################################

# Common prefix for flags/options.
CLIOPT_PREFIX = "-"

################################################################################
# Results of parsing.
class CliOptToken(Enum):
    FLAG    = 0
    OPTION  = 1
    FILE    = 2
    NO_OPT  = 3
    INVALID = 4

################################################################################
# CLI parser for arguments.
class CliOpt():
    # Internal Variables.
    m_args    = [] # lst: input arguments being parsed
    m_idx     = 0  # int: index of next argument to parse
    m_flags   = [] # lst: list of flag strings (no prefix)
    m_options = [] # lst: list of option strings (ie their flags- no prefix)

    # Outward Variables.
    m_type  = CliOptToken.INVALID # type of token identified
    m_tkn   = ""                  # token in question (flag, option, file)
    m_value = ""                  # value tied to option (as applicable)

    ############################################################################
    # Init instance- prep to iterate over arguments.
    def __init__(self, args: List[str], flags: List[str], options: List[str]):
        # Prep internals.
        self.m_args    = args
        self.m_idx     = 0
        self.m_flags   = flags
        self.m_options = options

        # Prep outwards.
        self.m_type  = CliOptToken.INVALID
        self.m_tkn   = ""
        self.m_value = ""

    ############################################################################
    # Iterate through arguments. Returns true on success- false otherwise.
    def nextOpt(self) -> bool:
        # Reset output.
        self.m_type  = CliOptToken.INVALID
        self.m_tkn   = ""
        self.m_value = ""

        # Determine if required to parse.
        doIter = self.m_idx < len(self.m_args)

        # Iterate as needed.
        if doIter:
            # Consume token.
            self.m_tkn = self.m_args[self.m_idx]
            self.m_idx += 1

            # Determine type.
            if self.m_tkn[0] == CLIOPT_PREFIX:
                # Remove prefix for convenience.
                self.m_tkn = self.m_tkn[1:]

                # Narrow down flag/option.
                isFlag   = self.m_tkn in self.m_flags
                isOption = self.m_tkn in self.m_options

                # Determine.
                if   isFlag:   self.m_type = CliOptToken.FLAG
                elif isOption: self.m_type = CliOptToken.OPTION
                else:          self.m_type = CliOptToken.INVALID
            else:
                self.m_type  = CliOptToken.FILE
            
            # Consume/check for option arg as needed.
            if self.m_type == CliOptToken.OPTION:
                if self.m_idx < len(self.m_args): # force short circuit check
                    if self.m_args[self.m_idx][0] != CLIOPT_PREFIX:
                        self.m_value = self.m_args[self.m_idx]
                        self.m_idx += 1
                if 0 == len(self.m_value): # hasn't been updated? Error
                    self.m_type  = CliOptToken.NO_OPT

        # Return if iteration was done.
        return doIter
