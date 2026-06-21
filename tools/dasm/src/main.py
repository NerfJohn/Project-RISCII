################################################################################
# main.py: head of "disassembler" tool/program for RISCII images.
# by John Eslinger
################################################################################

import sys
import int16
import decode

def main():
    print("Hello, World!")

    print("Total arguments:", len(sys.argv))
    print("Script name:", sys.argv[0])
    print("Arguments:", sys.argv[1:])

    # Create a bytes object
    data = bytes([0x43, 0xFF])

    foo = int16.Int16(data)
    print(foo)
    print(foo.as_int())
    print(foo.as_uint())
    print(foo.as_hex())
    print(decode.__get_type(foo.as_uint()))

if __name__ == "__main__":
    main()