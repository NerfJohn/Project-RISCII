# PR3000: Assember-Linker (asmld) Manual
*"Program to assemble and link RISCII programs into a binary image"*

**Purpose**

Define the external workings and usage of asmld.exe, namely for use in creating binary images for RISCII hardware.

**Overview**

> **TODO:** At present, this document covers details currently being developed/heavily worked on. Writings present are draft notes and thus may or may not reflect the final result.

**Common Terms**

|Term             |Description                                         |
|-----------------|----------------------------------------------------|
|asmld, asmld.exe |assembler-linker program, topic of this manual      |

**Base Documents**

|Document Number  |Description                                         |
|-----------------|----------------------------------------------------|
|PR2000           |RISCII instruction set architecture                 |
|PR2001           |RISCII microprocessor user/programming manual       |

---

## Standards

The asmld.exe program creates images that adhere to both the RISCII microprocessor and ISA constraints (see documents PR2001 and PR2000, respectively).

## Assembly Language

The language includes variable arguments. Below is a summary of arguments:

|Argument Name|Syntax               |Description                                 |
|-------------|---------------------|--------------------------------------------|
|flag         |`%` + lowercase chars|sets special flags in instructions          |
|register     |`$` + 0-7            |register directly accessible in instructions|
|immediate    |integer (with `-`)   |constant value                              |
|array        |intgers in `{}`      |contiguous block of words, space delimited  |

The language is made up of statements- namely instructions and data initializers. Both are appended to their section in the order they appear. See below for details.

The language includes comments. They begin with `;` and carry on to the end of the line. They contribute nothing to the created image.

### Data statement

Begins with `.data` keyword followed by an integer or array. All data integers are word length. In arrays, words are appended from left to right. See examples

```
.data 5        ; 0x0005 appended
.data {-1 0 4} ; 0xFFFF00000004 appended
```

### Instruction statement

Instructions begin with a specific instruction keyword followed by arguments specific to the instruction. See quick guide table below.

|Code|Flags |Register|Register|Register|Immediate|Description              |
|----|------|--------|--------|--------|---------|-------------------------|
|AND |      |DST     |SR1     |SR2     |         |Bitwise AND (VAR)        |
|AND |      |DST     |SR1     |        |5bImm    |Bitwise AND (IMM)        |
|ORR |      |DST     |SR1     |SR2     |         |Bitwise ORR (VAR)        |
|ORR |      |DST     |SR1     |        |5bImm    |Bitwise ORR (IMM)        |
|XOR |      |DST     |SR1     |SR2     |         |Bitwise XOR (VAR)        |
|XOR |      |DST     |SR1     |        |5bImm    |Bitwise XOR (IMM)        |
|SHL |      |DST     |SR1     |SR2     |         |Shift Logic Left (VAR)   |
|SHL |      |DST     |SR1     |        |4bIm     |Shift Logic Left (IMM)   |
|SHR |a     |DST     |SR1     |SR2     |         |Shift Right (VAR)        |
|SHR |a     |DST     |SR1     |        |4bIm     |Shift Right (IMM)        |
|ADD |      |DST     |SR1     |SR2     |         |Arithmetic Add (VAR)     |
|ADD |      |DST     |SR1     |        |5bImm    |Arithmetic Add (IMM)     |
|SUB |      |DST     |SR1     |SR2     |         |Arithmetic Subtract (VAR)|
|SUB |      |DST     |SR1     |        |5bImm    |Arithmetic Subtract (IMM)|
|LBI |s     |DST     |        |        |8bImmVal |Load Byte Immediate      |
|LDR |      |DST     |SR1     |        |6bOfst   |Load Memory Word         |
|STR |      |SRC     |SR1     |        |6bOfst   |Store Memory Word        |
|BRC |nzpc\*|        |        |        |8bImmVal |Branch Conditional       |
|JPR |      |SR1     |        |        |5bImm    |Jump Register            |
|JPR |r\*   |        |        |        |         |Return From Interrupt    |
|JLR |      |DST     |SR1     |        |5bImm    |Jump/Link Register       |
|SWP |      |DST     |SR1     |        |6bOfst   |Swap Register Memory     |
|NOP |      |        |        |        |         |No Operation             |
|HLT |      |        |        |        |         |Halt/Pause               |

*\*BRC requires at least one flag; JPR flag is required if no other arguments are given.*
