# PR2000: Instruction Set Architecture
*"Interface between RISCII software and hardware"*

**Purpose**

Standard interface for hardware/software designs to be based off of while ensuring they can work together smoothly.

As needed, this is v1.0.0 of the RISCII ISA.

**Overview**

- [Memory Architecture](#memory-architecture)
	- [Core processor](#core-processor)
	- [Random Access Memory](#random-access-memory)
- [State Machine](#state-machine)
- [Instruction Set](#instruction-set)
	- [Summary](#summary)
	- [Detailed Description](#detailed-description)
	- [Condition Codes](#condition-codes)


**Common Terms**

|Term             |Description                                         |
|-----------------|----------------------------------------------------|
|ISA              |Instruction Set Architecture                        |
|word (length)    |16 bits wide                                        |
|text memory      |64 KB address space for instruction storage         |
|data memory      |64 KB address space for data storage + mapping      |
|immediate (value)|constant binary value- often varies in bit width    |
|sext(), zext()   |signed, zero extended value- extended to word length|
|int16            |word length signed integer- usually for interpreting|

---

## Memory Architecture

### Core Processor

Instructions can directly access a 8 word-length register file. These registers are referred to `R0` thru `R7`. Due to this, operands and results are word length.

It is expected hardware also supports the following registers.

|Name|Full Name      |Description                                              |
|----|---------------|---------------------------------------------------------|
|PC  |program counter|address of the instruction being fetched from text memory|
|CC  |condition codes|flags describing result of previous operation            |

The PC value is used to fetch the next instruction to execute (typically being incremented by 2- the number of bytes per word). The CC register contains the flags `nzpc` and are used for conditional branching. See [Instruction Set](#instruction-set) for more details).

### Random Access Memory

Instructions and Data use separate address spaces (referred to text and data memory, respectively). Each space is addressed with word-length addresses (ie 64 KB spaces). Each space is byte addressable, though is accessed by word (see [Instruction Set](#instruction-set)). Due to this, addresses are rounded down to the nearest aligned word.

Text memory has the following layout:

|Address Range  |Range Name|Access|Description                |
|---------------|----------|------|---------------------------|
|0x0000 - 0xFFFF|program   |Rd    |program's instruction space|

Data memory has the following layout:

|Address Range  |Range Name|Access|Description                |
|---------------|----------|------|---------------------------|
|0x0000 - 0xBFFF|free      |Rd/Wr |program's data space       |
|0xC000 - 0xFFFF|mapped    |Rd/Wr |memory mapped hardware     |

The top quarter of data memory is reserved for memory mapped hardware registers. Exact usage of the space (whether tied to hardware, generic memory, or nothing) is hardware specific.

## State Machine

The high-level hardware has the following states:

|State|Full Name  |Execute Instructions|Allow Pausing|Allow Interrupts|
|-----|-----------|--------------------|-------------|----------------|
|PSE  |pause      |No                  |-            |-               |
|INT  |interrupt  |Yes                 |Yes          |No              |
|APP  |application|Yes                 |Yes          |Yes             |

PSE operates at a higher precedence than INT or APP- creating a hierarchical state machine. INT and APP are similar, except INT blocks any interrupt from taking control (pausing is separate from interrupts). See state logic below.

```
if   (external or internal pause)? {state = PSE;}
elif (new or servicing interrupt)? {state = INT;}
else                               {state = APP;}
```

## Instruction Set

Each instruction is word length and stored in text memory (word aligned). Instructions are designed to primarily operator on word length values.

The following abbreviations are used to denote common fields in instructions:

|Abbreviation       |Description                                      |
|-------------------|-------------------------------------------------|
|DST                |Destination register for result of operation     |
|SR1                |Source register for first operand for operation  |
|SR2                |Source register for second operand for operation |
|SRC                |Source register for value to write to data memory|
|4bIm               |4-bit immediate value                            |
|5bImm              |5-bit immediate value                            |
|6bOfst             |6-bit immediate value                            |
|8bImmVal           |8-bit immediate value                            |
|(lowercase letters)|flags with instruction specific effects          |

### Summary

|Operation Name           |Abbreviation |Bit Field Summary     |
|-------------------------|-------------|----------------------|
|Bitwise AND (VAR)        |AND          |1111-DST-SR1-000-SR2  |
|Bitwise AND (IMM)        |AND          |1111-DST-SR1-1-5bImm  |
|Bitwise OR (VAR)         |ORR          |1110-DST-SR1-000-SR2  |
|Bitwise OR (IMM)         |ORR          |1110-DST-SR1-1-5bImm  |
|Bitwise XOR (VAR)        |XOR          |1011-DST-SR1-000-SR2  |
|Bitwise XOR (IMM)        |XOR          |1011-DST-SR1-1-5bImm  |
|Shift Logic Left (VAR)   |SHL          |1100-DST-SR1-000-SR2  |
|Shift Logic Left (IMM)   |SHL          |1100-DST-SR1-10-4bIm  |
|Shift Right (VAR)        |SHR          |1101-DST-SR1-0-a-0-SR2|
|Shift Right (IMM)        |SHR          |1101-DST-SR1-1-a-4bIm |
|Arithmetic Add (VAR)     |ADD          |1000-DST-SR1-000-SR2  |
|Arithmetic Add (IMM)     |ADD          |1000-DST-SR1-1-5bImm  |
|Arithmetic Subtract (VAR)|SUB          |1001-DST-SR1-000-SR2  |
|Arithmetic Subtract (IMM)|SUB          |1001-DST-SR1-1-5bImm  |
|Load Byte Immediate      |LBI          |1010-DST-s-8bImmVal   |
|Load Memory Word         |LDR          |0101-DST-SR1-6bOfst   |
|Store Memory Word        |STR          |0100-SRC-SR1-6bOfst   |
|Branch Conditional       |BRC          |0010-nzpc-8bImmVal    |
|Jump Register            |JPR          |0110-000-SR1-0-5bImm  |
|Return From Interrupt    |RTI          |0110-000-000-1-00000  |
|Jump/Link Register       |JLR          |0111-DST-SR1-0-5bImm  |
|Swap Register Memory     |SWP          |0001-DST-SRC-6bOfst   |
|No Operation             |NOP          |0000-000000000000     |
|Halt/Pause               |HLT          |0011-000000000000     |

### Detailed Description

***Bitwise AND (AND)***

- 1111-DST-SR1-000-SR2
- 1111-DST-SR1-1-5bImm

Performs bitwise AND operation between registers SR1 and SR2, or between register SR1 and `sext(5bImm)`. Result is placed in register DST.

***Bitwise OR (ORR)***

- 1110-DST-SR1-000-SR2
- 1110-DST-SR1-1-5bImm

Performs bitwise OR operation between registers SR1 and SR2, or between register SR1 and `sext(5bImm)`. Result is placed in register DST.

***Bitwise XOR (XOR)***

- 1011-DST-SR1-000-SR2
- 1011-DST-SR1-1-5bImm

Performs bitwise XOR operation between registers SR1 and SR2, or between register SR1 and `sext(5bImm)`. Result is placed in register DST.

***Shift Logic Left (SHL)***

- 1100-DST-SR1-000-SR2
- 1100-DST-SR1-10-4bIm 

Performs shift logic left operation between registers SR1 and SR2, or between register SR1 and `zext(4bIm)`. Result is placed in register DST.

***Shift Right (SHR)***

- 1101-DST-SR1-0-a-0-SR2
- 1101-DST-SR1-1-a-4bIm

Performs shift right operation between registers SR1 and SR2, or between register SR1 and `zext(4bIm)`. Result is placed in register DST.

Flag `a` determines shift type: set performs shift arithmetic right, cleared performs shift logic right.

***Arithmetic Add (ADD)***

- 1000-DST-SR1-000-SR2
- 1000-DST-SR1-1-5bImm

Performs arithmetic add operation between registers SR1 and SR2, or between register SR1 and `sext(5bImm)`. Result is placed in register DST.

***Arithmetic Subtract (SUB)***

- 1001-DST-SR1-000-SR2
- 1001-DST-SR1-1-5bImm

Performs arithmetic subtract of registers SR1 from SR2, or register SR1 from `sext(5bImm)`. Result is placed in register DST.

*For emphasis: register SR1 is subtracted from the other operand value.*

***Load Byte Immediate (LBI)***

- 1010-DST-s-8bImmVal

Computes a value based on `8bImmVal`. Result is placed in register DST.

Flag `s` determines computation: set performs `(DST << 8) | 8bImmVal`, cleared performs `sext(8bImmVal)`.

***Load Memory Word (LDR)***

- 0101-DST-SR1-6bOfst

Loads one word into DST from memory address `SR1 + sext(6bOfst << 1)`.

***Store Memory Word (STR)***

- 0100-SRC-SR1-6bOfst

Stores register SRC to memory address `SR1 + sext(6bOfst << 1)`.

***Branch Conditional (BRC)***

- 0010-nzpc-8bImmVal

Sets PC to `PC + 2 + sext(8bImmVal << 1)` if certain conditions are met. If all conditions are not met, instruction has no effect.

The conditions to set the PC are as follows:

1. For flags `nzp`, both hardware and instruction have at least one of the same flag both set.
2. If instruction's flag `c` is set, the hardware's flag `c` is also set.

For more information about the conditions and flags, see [Condition Codes](#condition-codes).

***Jump Register (JPR)***

- 0110-000-SR1-0-5bImm

Sets PC to `SR1 + sext(5bImm << 1)`.

***Return From Interrupt (RTI)***

- 0110-000-000-1-00000

Switches state machine from INT to APP state. If not in the INT state, instruction has no effect.

***Jump/Link Register (JLR)***

- 0111-DST-SR1-0-5bImm

Sets PC to `SR1 + sext(5bImm << 1)` while also saving `PC + 2` to register DST.

***Swap Register Memory (SWP)***

- 0001-DST-SRC-6bOfst

Swaps values of register DST with memory address `SR1 + sext(6bOfst << 1)`.

***No Operation (NOP)***

- 0000-000000000000

Instruction has no effect.

***Halt/Pause (HLT)***

- 0011-000000000000

Forces hardware to enter the PSE state. PC is set to `<address of HLT instruction> + 2`.

### Condition Codes

Each condition code flag, when set, infers data about a given word length value. The table belows describes each flags meaning.

|Flag|Flag Name|Meaning If Set                                      |
|----|---------|----------------------------------------------------|
|n   |negative |Last data operation result, as int16, was negative  |
|z   |zero     |Last data operation result, as int16, was zero      |
|p   |positive |Last data operation result, as int16, was positive  |
|c   |carry    |Last data operation, as subtract, had carry-out     |

Hardware is expected to generate and store these flags based on the result saved to register DST after the following operations:

- AND
- ORR
- XOR
- SHL
- SHR
- ADD
- SUB
- LBI

When determining if conditions are met, the flags between the BRC instruction and hardware's stored version are compared. The comparision logic is below

```
nzpMatch = (bool)(brcNzp & hardwareNzp)    // BRC and HW must have matching nzp
cMatch   = (bool)(brcC ? hardwareC : true) // if BRC c set, then HW c must be set
COND_MET = nzpMatch & cMatch
```

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

*Note: Flag `c` is generally only considered valid after a SUB instruction.*
