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
    data = bytes([0x61, 0xdF])

    foo = int16.Int16(data)
    print(foo)
    print(foo.as_int())
    print(foo.as_uint())
    print(f"0x{foo.as_hex()}")
    print(decode.__get_type(foo.as_uint()))
    print(f"%{decode.__get_flags(foo.as_uint())}")

if __name__ == "__main__":
    main()