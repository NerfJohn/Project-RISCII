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

    img = image.BinImage()
    r = image.parse_image(sys.argv[1], img)
    print (r)
    print(img.text)
    print(img.data)

    foo = int16.Int16(img.text)
    print(foo.as_hex())
    print(decode.__get_type(foo.as_uint()))
    print(decode.__get_flags(foo.as_uint()))

if __name__ == "__main__":
    main()