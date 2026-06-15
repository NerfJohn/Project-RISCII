################################################################################
# main.py: head of "disassembler" tool/program for RISCII images.
# by John Eslinger
################################################################################

import sys

def main():
    print("Hello, World!")

    print("Total arguments:", len(sys.argv))
    print("Script name:", sys.argv[0])
    print("Arguments:", sys.argv[1:])

if __name__ == "__main__":
    main()