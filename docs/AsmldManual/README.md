# Project RISCII Assembler/Linker Manual
*"Reference manual for 'asmld.exe' program"*

---

This README is a summary on how to use Project RISCII's assembler/linker program (asmld.exe). This manual covers the following material:
- Basic Usage
- Assembly Language Syntax
- Warnings and Errors

## Basic Usage
---
The assembler/linker (hereafter referenced just as "assembler") is used to convert one or more files written in assembly language (specified below) into one binary image executable by the RISCII microprocessor.

### Command Line Usage

The assembler is directly called via the command line. It takes files (or paths to single files) written in assembly language as arguments. It also takes special flags as arguments, too. These arguments (and their effects) are as follows:
- "-h": prints out help menu and exits successfully
- "-o \<arg\>": uses "arg" as name for output binary image file
- "-ll \<arg\>": sets stdout log level to:
	- error: print errors only
	- warning: print warnings and errors
	- info: process related info
	- debug: all available output
- "-we": treat warnings as errors (warnings print with error priority)

By default, the assembler prints all errors and warnings only, where warnings are acceptable and the name of the binary image is derived from the first assembly file given (same name with ".bin" file type).

### Additional Info

When an error (or warning when flag "-we" is specified) is encountered, the binary image is not created. Instead, an error message is printed to stdout and the assembler exits with a return code indicating the offending error.

For more information about the created binary image (ie how it is formatted), see documentation of the RISCII microprocessor.

## Assembly Language Syntax

---

The assembly language is a typed programming language providing very low-level access to the RISCII hardware (language is specific to the hardware). Language allows for niche low-level firmware creation and view of compiled "high-level" firmware.

### Variable Arguments

Many assembly language constructs use configurable arguments. The following variable arguments are available:
- flag value: "%" followed by 1+ lowercase chars
- register value: "$" followed by a non-negative, decimal number
- immediate value: decimal or hexadecimal value
- string literal: one line string enclosed by quotes (valid escape chars include):
	- '\0': null
	- '\\\\': backslash
	- '\\\"': quote
	- '\n': newline
	- '\r': return
	- '\t': tab
- label value: group of letters, numbers, and/or underscores
- array value: 1+ variable arguments (besides array values)

Variable resolution depends on the context. In general, arguments are resolved to fit within one word (word size is defined by the RISCII instruction set architecture). String literals are packed into words such that each char takes up 1 byte, with "leftmost" chars being placed at lower addresses (using the least significant bits if packed). Arrays have each element resolve its size first, before similarly placing "leftmost" elements occuring at lower addresses. If a string literal or array does not align to the architecture's word size, packing bytes (equal to 0x00) are appended.

### Instructions

Instructions are used to specify binary instructions to place in the bniary image (ie instructions for the RISCII microprocessor to execute). Instructions are invoked using the instruction's keyword followed by a valid set of variable arguments (separated by whitespace). Below are valid instruction/variable combinations:

|Keyword|-Flags|-Register 1|-Register 2|-Register 3|-Immediate|-Description|
|-------|------|-----------|-----------|-----------|----------|------------|
| AND   |      | Dest      | Source 1  | Source 2  |          | Bitwise AND|
| AND   |      | Dest      | Source 1  |           | Source 2 | Bitwise AND|
| ORR   |      | Dest      | Source 1  | Source 2  |          | Bitwise ORR|
| ORR   |      | Dest      | Source 1  |           | Source 2 | Bitwise ORR|
| XOR   |      | Dest      | Source 1  | Source 2  |          | Bitwise XOR|
| XOR   |      | Dest      | Source 1  |           | Source 2 | Bitwise XOR|
| SHL   |      | Dest      | Source 1  | Source 2  |          | Shift Left |
| SHL   |      | Dest      | Source 1  |           | Source 2 | Shift Left |
| SHR   | a    | Dest      | Source 1  | Source 2  |          | Shift Right|
| SHR   | a    | Dest      | Source 1  |           | Source 2 | Shift Right|
| ADD   |      | Dest      | Source 1  | Source 2  |          | Addition   |
| ADD   |      | Dest      | Source 1  |           | Source 2 | Addition   |
| SUB   |      | Dest      | Source 1  | Source 2  |          | Subtraction|
| SUB   |      | Dest      | Source 1  |           | Source 2 | Subtraction|
| LBI   | s    | Dest      |           |           | Source   | Load Byte  |
| LDR   |      | Dest      | Pointer   |           | Offset   | Load Word  |
| STR   |      | Source    | Pointer   |           | Offset   | Store Word |
| BRC   | nzpc\*|          |           |           | Offset   | Branch     |
| JPR   |      | Pointer   |           |           | Offset   | Jump       |
| JPR   | r    |           |           |           |          | INT Return |
| JLR   |      | Dest      | Pointer   |           | Offset   | Jump/Link  |
| NOP   |      |           |           |           |          | No Op      |
| HLT   |      |           |           |           |          | Halt       |

\* Flags are optional, but at least one of the flags for BRC must be given.

For more information about the lengths/sizes available for each argument, see documentation for the RISCII instruction set architecture.

### Functions

The assembler provides various "functions" that appear as one instruction in assembly, though expand to multiple instructions in the binary image. They allow the assembly language to support operations not supported by instructions directly.

At present, the only function available is the "Load Address" function. The function generates two LBI instructions to load an address (specified by a label) into a given register. The function is formatted below:

<code> \_la \<register:dest\> \<label:address\> </code>

### Global Data

Global data is used to reserve RAM memory for data storage during runtime. This data can be optionally initialized (though at the cost of needing to be saved to the binary image).

To reserve and initialize data, the ".data" keyword is used, followed by a decimal or hexadecimal integer, a string literal, label, or array of the previously mentioned variable arguments. Examples are as follows:

<code>.data 5</code>\
<code>.data "Hello"</code>\
<code>.data myLabel</code>\
<code>.data {0xA9 "World!" label1}</code>

To reserve data without initializing, the ".bss" keyword is used, followed by a non-negative decimal or hexadecimal integer. This reserves the specified amount of bytes (rounded up to align with the hardware's word size) without initializing the values beforehand. Examples are as follows:

<code>.bss 10</code>\
<code>.bss 0x20</code>

### Labels

Labels are used to "name" certain instructions and global data so they can be referenced later.

Labels are declared by writing the label name followed by a colon just before an instruction, function, or global data reservation. See examples below:

<code>f_myFunc:\
      SHR %a $0 $1 13</code>\
<code>\_getAddr:\
       \_la $7 \_getAddr\
<code>foobar2: .data foobar2</code>\
<code>my_Label:\
      .bss 10</code>

Labels can then be referenced by certain functions and global data initializations (see examples above). Upon assembling, the labels are converted into addresses can placed in the binary image anywhere the label was referenced.

The assembler predefines/declares several labels for the user. These label names can be referenced, but not defined, by the user (except for "__START", which the user must define to create a binary image). These special labels are as follows:
- "__START": label defines first instruction of program (user must define)
- "__BSS": first address of the uninitialized data section
- "__FREE": first address of unreserved RAM memory
- "__SIZE": size of the binary image in bytes

### Other Features

The assembly language supports comments. Comments are decorative/informative strings that do not affect the created binary image in any way. They begin with a semicolon and continue until the next newline. See below for examples:

<code>; This is a comment</code>\
<code>NOP ; "NOP" is an instruction- this text is not</code>

## Warnings and Errors

---

Various warnings and errors are defined by the assembler. The assembler emits explanations and return codes for each. For posterity, those codes and explanations are repeated below.

|Warning Code|Description|
|------------|-------------------------------------------------------------|
|0x40        | Flag value has 2+ of specific char- repetition has no effect|
|0x41        | Label is defined, but never used- extra label has no effect |

|Error Code|Description|
|----------|---------------------------------------------------------------|
|0x80      | File specified on CLI could not be opened or read             |
|0x81      | Sequence of chars in files couldn't be recognized as assembly |
|0x82      | Expected a specific token- next token was not that token      |
|0x83      | Next token was not in set of expected tokens                  |
|0x84      | Char given in flag value is not supported in current context  |
|0x85      | Number does not refer to any known register                   |
|0x86      | Integer was not in range supported by current context         |
|0x87      | Number of instructions/functions cannot fit in binary image   |
|0x88      | Binary image is too big for target hardware                   |
|0x89      | Could not create or open binary image file                    |
|0x8A      | No files are given to the assembler (via CLI) to assemble/link|
|0x8B      | Argument for CLI flag was invalid                             |
|0x8C      | CLI flag specified is not recognized by the assembler         |
|0x8D      | CLI flag requires an argument, but none was given             |
|0x8E      | Label is defined in multiple places between the input files   |
|0x8F      | Label was defined, but not given an instruction/data to tie to|
|0x90      | Label was referenced, but never given a definition to refer to|
|0x91      | Target's RAM cannot hold all the reserved global data         |
|0x92      | "__START" label does not refer to an instruction or function  |
|0x93      | Escape char in string literal is unrecognized by assembler    |