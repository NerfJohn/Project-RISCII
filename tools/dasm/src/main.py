################################################################################
# main.py: head of "disassembler" tool/program for RISCII binary images.
################################################################################

from LevelLog import LevelLog, LevelLogLevel as Lvl

################################################################################
# Root of program- conducts main procedure.
def main():
    a = LevelLog(Lvl.DEBUG)

    a.print(Lvl.SILENT, "silent")
    a.print(Lvl.ERROR, "error")
    a.print(Lvl.WARNING, "warning")
    a.print(Lvl.INFO, "info")
    a.print(Lvl.DEBUG, "debug")

    a.setLevel(Lvl.WARNING)
    print(a)

    a.print(Lvl.SILENT, "silent")
    a.print(Lvl.ERROR, "error")
    a.print(Lvl.WARNING, "warning")
    a.print(Lvl.INFO, "info")
    a.print(Lvl.DEBUG, "debug")

################################################################################
# Starting point of program.
if __name__ == "__main__":
    main()
