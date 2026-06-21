################################################################################
# decode.py: ISA specific decoding of int16 chunks.
# by John Eslinger
################################################################################

# Common ISA opcodes.
__opcodes = {
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

################################################################################
# Interpret 16-bit word into instruction type (based on opcode).
def __get_type(word: int):
    # Extract opcode.
    opcode = (0xF000 & word) >> 12

    # Determine opcode.
    return __opcodes.get(opcode, lambda: None)

################################################################################
# Interpret flags from instruction (based on opcode).
def __get_flags(word: int):
    # Extract type.
    opcode = (0xF000 & word) >> 12
    itype  = __opcodes.get(opcode, lambda: None)
    if itype == None: return None # "short circuit" safety

    # Determine (assembly explicit) flags.
    flags = ""
    if   itype == "SHR" and (0x0010 & word): flags = "a"
    elif itype == "LBI" and (0x0100 & word): flags = "s"
    elif itype == "JPR" and (0x0020 & word): flags = "r"
    elif itype == "BRC":
        if (0x0800 & word): flags += "n"
        if (0x0400 & word): flags += "z"
        if (0x0200 & word): flags += "p"
        if (0x0100 & word): flags += "c"
    return flags

################################################################################
# Interpret immediate for configurable size and sign.
def __get_imm(word: int, bits: int, signed: bool):
    # Extract (immediate always LSb justified).
    mask = (0xFFFF & ~(0xFFFF << bits))
    imm  = mask & word

    # Apply sign.
    return imm - (mask + 1) if signed and (imm >> (bits - 1)) else imm

################################################################################
# Interpret register number for configurable LSb.
def __get_reg(word: int, lsb: int):
    # Extract.
    return 0x0007 & (word >> lsb)