################################################################################
# main.py: head of "disassembler" tool/program for RISCII images.
# by John Eslinger
################################################################################

import sys
import int16

def main():
    print("Hello, World!")

    print("Total arguments:", len(sys.argv))
    print("Script name:", sys.argv[0])
    print("Arguments:", sys.argv[1:])

    # Create a bytes object
    data = bytes([0xDE, 0xAD, 0x0E, 0xEF])

    foo = int16.Int16(data)
    print(foo)
    print(foo.as_int())
    print(foo.as_hex())

    bar = foo.copy()
    print(bar)
    print(bar.as_int())
    print(bar.as_hex())


if __name__ == "__main__":
    main()