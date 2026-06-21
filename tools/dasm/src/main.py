################################################################################
# main.py: head of "disassembler" tool/program for RISCII images.
# by John Eslinger
################################################################################

import sys
import int16
import decode
import image

def main():
    print("Hello, World!")

    print("Total arguments:", len(sys.argv))
    print("Script name:", sys.argv[0])
    print("Arguments:", sys.argv[1:])

    r = image.parse_image(sys.argv[1])
    print(r.readFile)
    print(r.resultMsg)
    print(r.text)
    print(r.data)

if __name__ == "__main__":
    main()