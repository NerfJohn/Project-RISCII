################################################################################
# main.py: head of "disassembler" tool/program for RISCII binary images.
################################################################################

import sys

from LevelLog import LevelLog, LevelLogLevel as Lvl
from CliOpt import CliOpt, CliOptToken as Tkn

################################################################################
# Root of program- conducts main procedure.
def main():
    a = LevelLog(Lvl.DEBUG)
    b = CliOpt(sys.argv[1:], ["h"], ["o", "ll"])

    while b.nextOpt():
        print(f"{b.m_type}, {b.m_tkn}, {b.m_value}")

################################################################################
# Starting point of program.
if __name__ == "__main__":
    main()
