# Instruction Set Architecture
*"Interface combining hardware and software components of computing"*

---

This README is a summary of the instruction set architecture (ISA for short) used by both the RISCII hardware and software components. These writings primarily focus on defining the instruction set, but also define other expectations of both the hardware and software. This ISA covers the following material:
- Architecture Requirements
- Memory Layout
- Instruction Set

## Architecture Requirements
---
The ISA requires hardware to follow a list of expectations in order to ensure proper interfacing with the software. These requirements are as follows:
- Instructions are 16-bit wide
- Addresses are 16-bits wide
- Operands and Results are 16-bits wide
- The core processor support eight 16-bit register in the register file
- The processor records condition codes (see section "Instruction Set")
- The processor records the current instruction address (ie program counter)
- The hardware implements 2 address spaces (see section "Memory Layout")
- The hardware accesses memory in word aligned fashion

In general, the hardware should implement a 16-bit architecture (where the term "word" is synonymous with 2-bytes). The hardware is also expected to implement memory/registers based on the instruction set's ability to access them.

## Memory Layout
---
The ISA enforces expectations on memory available to the software (and provided by hardware). The ISA defines two address spaces:
- program address space: 64 KB memory for storing instructions
- data address space: 64 KB memory for data and mapped registers 

These two address spaces follow the Harvard architecture principle of being completely separate. That is, the address spaces to not directly "overlap" by sharing words.

Likewise, in line with the architecture requirements, it is expected that each address space is accessed with at least one word (ie 2 byte) precision (with accesses themselves being word aligned).

For the data address space, while addresses 0x0000-0x7FFF should access generic use memory, addresses 0x8000-0xFFFF are reserved for memory mapped registers (exact details specific to the hardware). In other words, while the data address space is 64 KB, only 32 KB are guaranteed to be generic use memory. The program address space is fully generic memory.

## Instruction Set
---
The core of the ISA is the instruction set, which defines how bits in the program address space are interpreted. In general, instructions are 16-bit values arranged as aligned words in the program address space (with "later" instructions placed at incrementally higher addresses).

### Instruction Set Summary

Below is a table summarizing the available instructions/operations:

|Operation Name        |Opcode Keyword|Bit Field Summary     |
|----------------------|--------------|----------------------|
|Bitwise AND           |AND           |1111-DST-SR1-000-SR2  |
|                      |              |1111-DST-SR1-1-5bImm  |
|Bitwise OR            |ORR           |1110-DST-SR1-000-SR2  |
|                      |              |1110-DST-SR1-1-5bImm  |
|Bitwise XOR           |XOR           |1011-DST-SR1-000-SR2  |
|                      |              |1011-DST-SR1-1-5bImm  |
|Shift Logical Left    |SHL           |1100-DST-SR1-000-SR2  |
|                      |              |1100-DST-SR1-10-4bIm  |
|Shift Right           |SHR           |1101-DST-SR1-0-a-0-SR2|
|                      |              |1101-DST-SR1-1-a-4bIm |
|Arithmetic Addition   |ADD           |1000-DST-SR1-000-SR2  |
|                      |              |1000-DST-SR1-1-5bImm  |
|Arithmetic Subtraction|SUB           |1001-DST-SR1-000-SR2  |
|                      |              |1001-DST-SR1-1-5bImm  |
|Load Byte Immediate   |LBI           |1010-DST-s-8bImmVal   |
|Load Memory Word      |LDR           |0101-DST-SR1-6bOfst   |
|Store Memory Word     |STR           |0100-SRC-SR1-6bOfst   |
|Branch Conditional    |BRC           |0010-nzpc-8bImmVal    |
|Jump Base Register    |JPR           |0110-000-SR1-0-5bImm  |
|Return From Interrupt |(JPR)         |0110-000-000-1-00000  |
|Jump and Link Register|JLR           |0111-DST-SR1-0-5bImm  |
|Swap Register Memory  |SWP           |0001-DST-SR1-6bOfst   |
|No Operation          |NOP           |0000-000000000000     |
|Halt/Pause            |HLT           |0011-000000000000     |

### Instruction Set Details

**_Bitwise AND (AND)_**
- 1111-DST-SR1-000-SR2
- 1111-DST-SR1-1-5bImm

Performs bitwise AND operation between registers SR1 and SR2, or between register SR1 and sign extended immediate 5bImm. Result is placed in register DST and condition codes are updated.

**_Bitwise OR (ORR)_**
- 1110-DST-SR1-000-SR2
- 1110-DST-SR1-1-5bImm

Performs bitwise OR operation between registers SR1 and SR2, or between register SR1 and sign extended immediate 5bImm. Result is placed in register DST and condition codes are updated.

**_Bitwise XOR (XOR)_**
- 1011-DST-SR1-000-SR2
- 1011-DST-SR1-1-5bImm

Performs bitwise XOR operation between registers SR1 and SR2, or between register SR1 and sign extended immediate 5bImm. Result is placed in register DST and condition codes are updated.

**_Shift Logical Left (SHL)_**
- 1100-DST-SR1-000-SR2
- 1100-DST-SR1-10-4bIm 

Performs shift logical left operation between registers SR1 and SR2, or between register SR1 and zero extended immediate 4bIm. Result is placed in register DST and condition codes are updated.

**_Shift Right (SHR)_**
- 1101-DST-SR1-0-a-0-SR2
- 1101-DST-SR1-1-a-4bIm

Performs shift right operation between registers SR1 and SR2, or between register SR1 and zero extended immediate 4bIm. Result is placed in register DST and condition codes are updated.

If flag "a" is set, a shift arithmetic right operation is performed. Otherwise, a shift logical right operation is performed.

**_Arithmetic Addition (ADD)_**
- 1000-DST-SR1-000-SR2
- 1000-DST-SR1-1-5bImm

Performs arithmetic addition operation between registers SR1 and SR2, or between register SR1 and sign extended immediate 5bImm. Result is placed in register DST and condition codes are updated.

**_Arithmetic Subtraction (SUB)_**
- 1001-DST-SR1-000-SR2
- 1001-DST-SR1-1-5bImm

Performs arithmetic subtraction operation between registers SR1 and SR2, or between register SR1 and sign extended immediate 5bImm. Result is placed in register DST and condition codes are updated.

Specifically the first operand, register SR1, is used for the right hand side of the operation (ie DST = SR2 - SR1 and/or DST = 5bImm - SR1).

**_Load Byte Immediate (LBI)_**
- 1010-DST-s-8bImmVal

Computes a value based on the immediate 8bImmVal. Result is placed in register DST and condition codes are updated.

If flag "s" is set, the new value is equivalent to register DST's old value left shifted by 8 bits (with immediate 8bImmVal shifted in from the right). Otherwise, the new value is sign extended immediate 8bImmVal.

**_Load Memory Word (LDR)_**
- 0101-DST-SR1-6bOfst

Loads a word from memory based on the sum of register SR1 and a computed value (value equivalent to immediate 6bOfst sign extended, then left shifted by 1 bit). Result is stored in register DST.

**_Store Memory Word (STR)_**
- 0100-SRC-SR1-6bOfst

Stores a word from register SRC to memory based on the sum of register SR1 and a computed value (value equivalent to immediate 6bOfst sign extended, then left shifted by 1 bit).

**_Branch Conditional (BRC)_**
- 0010-nzpc-8bImmVal

Jumps to a new instruction address if certain conditions are met. New instruction address is the sum of the current instruction's address plus two and a computed value (value equivalent to immediate 8bImmVal sign extended, then left shifted by 1 bit). If the conditions are not met, then the instruction has no effect.

The conditions to jump to the new instruction address are as follows:
- processor and instruction must both have matching set flag between flags "nzp"
- if instruction flag "c" is set, then the processor's "c" flag must be set

See below subsection for more details about the meaning and useful combinations of flags "nzpc"

**_Jump Base Register (JPR)_**
- 0110-000-SR1-0-5bImm
- 0110-000-000-1-00000

Jumps to a new instruction address. New address is the sum of register SR1 and a computed value (value equivalent to immediate 5bImm sign extended, then left shifted by 1 bit).

If bit 5 if set (hereby referred to as flag "r"), then the instruction instead attempts to return from an interrupt (exact details are hardware specific). If interrupts are not supported (or hardware is not running an interrupt), then setting flag "r" instead runs the equivalent of a NOP instruction. If flag "r" is not set, the jump behavior described above is executed.

**_Jump and Link Register (JLR)_**
- 0111-DST-SR1-0-5bImm

Jumps to a new instruction address while saving the sum of the current instruction's address plus two. New address is the sum of register SR1 and a computed value (value equivalent to immediate 5bImm sign extended, then left shifted by 1 bit). The old address (plus two) is saved to register DST.

**_Swap Register Memory (SWP)_**
- 0001-DST-SR1-6bOfst

Swaps the values of register SR1 with a memory address. The memory address is the sum of register SR1 and a computed value (value equivalent to immediate 6bOfst sign extended, then left shifted by 1 bit). The previous memory value is stored in register DST and the previous register DST value is stored at the memory address.

**_No Operation (NOP)_**
- 0000-000000000000

Instruction performs no operation of note or circumstance. Beyond taking memory to store and time to run, instruction has no effect on the hardware or software.

**_Halt/Pause (HLT)_**
- 0011-000000000000

Puts the processor into a halted or paused state (exact details are hardware specific). Instruction should prevent next or future instructions from running. In general, hardware interference should be required to pass through/around this instruction.

### Branch Flag Details

The BRC instruction defined 4 flags "nzpc". Both the instruction and the processor are expected to have these flags, with their meaning (when set) as follows:

|Flag Name|Flag Letter|Meaning If Set                                      |
|---------|-----------|----------------------------------------------------|
|Negative |n          |Last data operation result, as int16, was negative  |
|Zero     |z          |Last data operation result, as int16, was zero      |
|Positive |p          |Last data operation result, as int16, was positive  |
|Carry    |c          |Last data operation (as SUB) resulted in carry-out  |

The processor's copy of these flags are updated everytime one of the following instructions are executed:
- AND
- ORR
- XOR
- SHL
- SHR
- ADD
- SUB
- LBI

Note that the carry flag "c", while updated for all the above instructions, is only meaningful/valid when the last data operation is a SUB instruction (ie flag "c" only reflects the carry-out status of arithmetic subtraction).

In general, these flags allow the software to conditionally jump for any comparison of 16-bit numbers. For convenience, the below table provides commonly checked conditions:

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
|nzpc     |SUB Op             |Carry-out occurred                          |
|nzp      |Any Op             |Instruction Relative Jump                   |
