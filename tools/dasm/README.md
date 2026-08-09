# RISCII Disassembler (dasm)
*"RISCII binary to text converter"*

> **TODO** This document is currently in progress/revision. Contents may be incomplete or contain semantic errors.

**Purpose**

To convert RISII binaries to assembly for debug/testing.

**Overview**

- [Goal](#goal)

**Common Terms**

|Term             |Description                                         |
|-----------------|----------------------------------------------------|
|dasm             |RISCII disassembler (topic of this document)        |
|word             |16-bit value                                        |

**Supporting Docs**

|Term             |Description                                         |
|-----------------|----------------------------------------------------|
|PR2000           |RISCII ISA (defines how to interpret instructions)  |
|PR2001           |uP Manual  (defines structure of binary)            |

---

## Goal

Program to convert RISCII binary to assembly/readable file. This allows for debugging created binaries and thus supports levels 3-5 (and partially level 2 for confirming test binaries).

### Inputs

Program takes 1 binary file and various command line arguments.

Binary file is organized into two contiguous sections- text and data (in that order) -made of big endian words. Each section consists of a single metadata word equal to the number of following words that are part of that section. Each section must consist of at least one non-metadata word. The max size of the binary is 64 KB (32,768 words). See outline below.

|Area Name    |# of Words|Description            |
|-------------|----------|-----------------------|
|text metadata|1         |# of instructions      |
|text values  |1-32765   |instructions of program|
|data metadata|1         |# of words of init data|
|data values  |1-32765   |pre-initialized data   |

Program takes in command line arguments- organized into flags, options, and files (see table below). Arguments can be given in any order. The program expects at least one file (ie the binary file).

|Argument|Syntax                               |Example   |
|--------|-------------------------------------|----------|
|flag    |-<1-2 letters>                       |-h        |
|option  |-<1-3 letters> <no '-' prefix string>|-o out.asm|
|file    |<no '-' prefix string>               |in.asm    |

### Outputs

Program- on a successful run -outputs an assembly file based on the input file. On an unsuccessful run, the program prints out the issue as an error.

Output file follows basic RISCII assembly format. Text section begins with `__START:` and contains each instruction using std argument ordering (ie `<opcode> <flags> <registers> <immediate> <label>`). Data section is expressed using a single data array (ie `.data (<word hex values>)`). 4-space tabs and newlines are used to format the file for readability. See example below.

```
__START:
ADD         $0  $2  $3
SUB         $1  $1      -10
BRC %nzpc               5
_la                             foobar
HLT

.data (
0xABCD  0x5555  0x0111  0x0000
0x0000  0xD0F0  0xFF00  0x0000
0x89AA  0xFFCC
)
```

Program output can be controlled using command line arguments. This includes both printed out outputs and file output. See examples below.

|CLI Argument |Description               |Arguments                          |
|-------------|--------------------------|-----------------------------------|
|-h           |print help menu + exit    |NA                                 |
|-o <filename>|set name of output file   |name of output file                |
|-ll <level>  |set verbosity of program  |SILENT, ERROR, WARNING, INFO, DEBUG|
|-v           |append raw hex values     |NA                                 |
|-a           |prepend assembly addresses|NA                                 |
