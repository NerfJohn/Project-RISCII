# Project RISCII: ISA (Instruction Set Architecture) (PR2000)
*"Interface between RISCII software and hardware"*

---

This document describes the semantics hardware and software must follow in order to work together. Most semantics are collaries of the instruction set. For arbitrary reasons, this document is a "microprocessor" document.

**Table of Contents**

- [Required Constraints](#h1)
- [Instruction Set](#h2)
- [Branch Condition Codes](#h3)

**Referenced Terms**

|Term            |Description                                              |
|----------------|---------------------------------------------------------|
|ISA             |Instruction Set Architecture                             |
|word (length)   |16 bits wide                                             |
|register        |memory unit, one word length                             |
|register file   |collection of registers accessible to by instructions    |
|condition codes |flags `nzpc`, see [Branch Condition Codes](#h3)          |
|interrupt mode  |unique mode/state of hardware, hardware defined meaning  |
|application mode|unique mode/state of hardware, hardware defined meaning  |
|paused mode     |unique mode/state of hardware, hardware defined meaning  |
|sext()          |sign extended operator, extend to one word length        |
|zext()          |zero extended operator, extend to one word length        |
|PC              |program counter, stores address of instruction           |

---

## Required Constraints {#h1}
---
This section details general constraints that must be followed. These constraints are inferred by the instruction set.

The constraints are as follows:
- All instructions are one word length
- All data (operand, result, memory access) are one word length
- All addresses (instruction, data) are one word length
- All memory is word aligned
- The hardware's register file supports 8 word length registers
- The hardware generates and stores condition codes
- The hardware supports interrupt and application modes
- The hardware supports a paused mode

## Instruction Set {#h2}
---
This section describes the instruction set. It specifies 1) what operations and available 2) what each operation does and 3) the bit field layout needed to invoke each operation.

### Instruction Set Summary

|Operation Name           |Bit Field Summary     |
|-------------------------|----------------------|
|Bitwise AND (VAR)        |1111-DST-SR1-000-SR2  |
|Bitwise AND (IMM)        |1111-DST-SR1-1-5bImm  |
|Bitwise OR (VAR)         |1110-DST-SR1-000-SR2  |
|Bitwise OR (IMM)         |1110-DST-SR1-1-5bImm  |
|Bitwise XOR (VAR)        |1011-DST-SR1-000-SR2  |
|Bitwise XOR (IMM)        |1011-DST-SR1-1-5bImm  |
|Shift Logic Left (VAR)   |1100-DST-SR1-000-SR2  |
|Shift Logic Left (IMM)   |1100-DST-SR1-10-4bIm  |
|Shift Right (VAR)        |1101-DST-SR1-000-SR2  |
|Shift Right (IMM)        |1101-DST-SR1-10-4bIm  |
|Arithmetic Add (VAR)     |1000-DST-SR1-000-SR2  |
|Arithmetic Add (IMM)     |1000-DST-SR1-1-5bImm  |
|Arithmetic Subtract (VAR)|1001-DST-SR1-000-SR2  |
|Arithmetic Subtract (IMM)|1001-DST-SR1-1-5bImm  |
|Load Byte Immediate      |1010-DST-s-8bImmVal   |
|Load Memory Word         |0101-DST-SR1-6bOfst   |
|Store Memory Word        |0100-SRC-SR1-6bOfst   |
|Branch Conditional       |0010-nzpc-8bImmVal    |
|Jump Register            |0110-000-SR1-0-5bImm  |
|Return From Interrupt    |0110-000-000-1-00000  |
|Jump/Link Register       |0111-DST-SR1-0-5bImm  |
|Swap Register Memory     |0001-DST-SR1-6bOfst   |
|No Operation             |0000-000000000000     |
|Halt/Pause               |0011-000000000000     |

### Instruction Set Details

**_Bitwise AND_**

- 1111-DST-SR1-000-SR2
- 1111-DST-SR1-1-5bImm

Performs bitwise AND operation between registers SR1 and SR2, or between register SR1 and `sext(5bImm)`. Result is placed in register DST.

**_Bitwise OR_**

- 1110-DST-SR1-000-SR2
- 1110-DST-SR1-1-5bImm

Performs bitwise OR operation between registers SR1 and SR2, or between register SR1 and `sext(5bImm)`. Result is placed in register DST.

**_Bitwise XOR_**

- 1011-DST-SR1-000-SR2
- 1011-DST-SR1-1-5bImm

Performs bitwise XOR operation between registers SR1 and SR2, or between register SR1 and `sext(5bImm)`. Result is placed in register DST.

**_Shift Logic Left_**

- 1100-DST-SR1-000-SR2
- 1100-DST-SR1-10-4bIm 

Performs shift logic left operation between registers SR1 and SR2, or between register SR1 and `zext(4bIm)`. Result is placed in register DST.

**_Shift Right_**

- 1101-DST-SR1-0-a-0-SR2
- 1101-DST-SR1-1-a-4bIm

Performs shift right operation between registers SR1 and SR2, or between register SR1 and `zext(4bIm)`. Result is placed in register DST.

Flag `a` determines shift type: set performs shift arithmetic right, cleared performs shift logic right.

**_Arithmetic Add_**

- 1000-DST-SR1-000-SR2
- 1000-DST-SR1-1-5bImm

Performs arithmetic add operation between registers SR1 and SR2, or between register SR1 and `sext(5bImm)`. Result is placed in register DST.

**_Arithmetic Subtract_**

- 1001-DST-SR1-000-SR2
- 1001-DST-SR1-1-5bImm

Performs arithmetic subtract of registers SR1 from SR2, or register SR1 from `sext(5bImm)`. Result is placed in register DST.

*For emphasis: register SR1 is subtracted from the other operand value.*

**_Load Byte Immediate_**

- 1010-DST-s-8bImmVal

Computes a value based on `8bImmVal`. Result is placed in register DST.

Flag `s` determines computation: set performs `sext(8bImmVal)`, cleared performs `(DST << 8) | 8bImmVal`.

**_Load Memory Word_**

- 0101-DST-SR1-6bOfst

Loads one word into DST from memory address `SR1 + sext(6bOfst << 1)`.

**_Store Memory Word_**

- 0100-SRC-SR1-6bOfst

Stores register SRC to memory address `SR1 + sext(6bOfst << 1)`.

**_Branch Conditional_**

- 0010-nzpc-8bImmVal

Sets PC to `PC + 2 + sext(8bImmVal << 1)` if certain conditions are met. If all conditions are not met, instruction has no effect.

The conditions to set the PC are as follows:
1. For flags `nzp`, both hardware and instruction have at least one of the same flag both set.
2. If instruction's flag `c` is set, the hardware's flag `c` is also set.

For more information about the conditions and flags, see [Branch Condition Codes](#h3).

**_Jump Register_**

- 0110-000-SR1-0-5bImm

Sets PC to `SR1 + sext(5bImm << 1)`.

**_Return From Interrupt_**

- 0110-000-000-1-00000

Returns hardware from interrupt mode to application mode if instruction is run while in interrupt mode. Otherwise, instruction has no effect.

**_Jump/Link Register_**

- 0111-DST-SR1-0-5bImm

Sets PC to `SR1 + sext(5bImm << 1)` while also saving `PC + 2` to register DST.

**_Swap Register Memory_**

- 0001-DST-SR1-6bOfst

Swaps values of register DST with memory address `SR1 + sext(6bOfst << 1)`.

**_No Operation_**

- 0000-000000000000

Instruction has no effect.

**_Halt/Pause_**

- 0011-000000000000

Forces hardware to enter paused mode.

## Branch Condition Codes {#h3}
---
This section describes the condition code (ie flags `nzpc`) in more detail, describing their calculation/meaning and when they're updated.

### Condition Code Usage

Each condition code flag, when set, infers data about a given word length value. The table belows describes each flags meaning.

|Flag Name|Flag Letter|Meaning If Set                                      |
|---------|-----------|----------------------------------------------------|
|Negative |n          |Last data operation result, as int16, was negative  |
|Zero     |z          |Last data operation result, as int16, was zero      |
|Positive |p          |Last data operation result, as int16, was positive  |
|Carry    |c          |Last data operation, as subtract, had carry-out     |

As mentioned under [Required Constraints](#h1), hardware is expected to generate and store these condition codes. Specifically, these codes should be generated and stored based on the result of the following operations:
- Bitwise AND
- Bitwise OR
- Bitwise XOR
- Shift Logic Left
- Shift Right
- Arithmetic Add
- Arithmetic Subtract
- Load Byte Immediate

*Note: that flag `c`, while updated for the above instructions, is only valid after an arithmetic subtract instruction is performed.*

### Common Condition Code Combinations

For convenience, common combinations of condition code flags (and their abstract meaning) are provided in the table below.

|Condition|Last Data Operation|Abstract Meaning                            |
|---------|-------------------|--------------------------------------------|
|n        |B - A              |If A > B (signed)                           |
|p        |B - A              |If A < B (signed)                           |
|nz       |B - A              |If A >= B (signed)                          |
|zp       |B - A              |If A <= B (signed)                          |
|npc      |A - B              |If A > B (unsigned)                         |
|npc      |B - A              |If A < B (unsigned)                         |
|nzpc     |A - B              |If A >= B (unsigned)                        |
|nzpc     |B - A              |If A <= B (unsigned)                        |
|z        |B - A              |A == B                                      |
|np       |B - A              |A != B                                      |
|nzpc     |Arithmetic Subtract|Carry-out occurred                          |
|nzp      |Any Operation      |Instruction Relative Jump                   |
