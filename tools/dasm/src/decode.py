################################################################################
# decode.py: ISA specific decoding of int16 chunks.
# by John Eslinger
################################################################################

################################################################################
# Interpret 16-bit word into instruction type (based on opcode).
def __get_type(word: int):
    # Extract opcode.
    opcode = (0xF000 & word) >> 12

    # ISA opcode definitions.
    opcodes = {
        0:  "NOP",
        1:  "SWP",
        2:  "BRC",
        3:  "HLT",
        4:  "STR",
        5:  "LDR",
        6:  "JPR",
        7:  "JLR",
        8:  "ADD",
        9:  "SUB",
        10: "LBI",
        11: "XOR",
        12: "SHL",
        13: "SHR",
        14: "ORR",
        15: "AND"
    }

    # Determine opcode.
    return opcodes.get(opcode, lambda: None)
