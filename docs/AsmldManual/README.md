# Project RISCII: Asmld (Assembler/Linker) Manual (PR3000)
*"Reference for design of the RISCII Assembler/Linker program"*

---

This document describes the capabilities and functions of the RISCII assembler/linker program (ie asmld.exe). It includes details about the RISCII assembly language and asmld.exe's specific implementation of it.

**Table of Contents**

- [RISCII Assembly Language](#riscii-assembly-language)
- [Assembler/Linker Program](#assemblerlinker-program)
- [Future Considerations](#future-considerations)

**Referenced Terms**

|Term            |Description                                              |
|----------------|---------------------------------------------------------|
|ISA             |"ISA" Document PR2000                                    |
|uP              |RISCII microprocessor (per "uPManual" Document PR2001)   |
|asmld.exe       |assembler/linker program (ie topic of this manual)       |
|assembly        |assembly language, namely the RISCII Assembly Language   |

---

## RISCII Assembly Language
---
This section describes the RISCII assembly language. It is formally defined by/on top of the ISA and merely implemented by asmld.exe. The language is meant to describe a binary program in its rawest readable form.

### Variable Arguments

Assembly includes different variables types, used as arguments for the language's different statements. Variables are often interpreted and validated based on their context.

***Flags***

Flags, consisting of lowercase letters prefixed with `%`, are used to infer set bit flags within instructions. Valid letters are instruction specific (see [Statement Types](#statement-types) below). See examples below.

```
%a      ; single flag
%npc    ; group of flags
```

***Registers***

Registers, consisting of a non-negative integer prefixed with `$`, are used to infer registers within the hardware's register file (see ISA document PR2000). Only `$0` through `$7` are valid. See examples below.

```
$0 $7    ; valid register range (as per ISA)
$4       ; single register
```

***Immediates***

Immediates represent constant integer values, represented as integer or hexadecimal values. Validity depends on the context (see [Statement Types](#statement-types) below), though integers are always read as signed values while hexadecimals are read as raw binary values. See examples below.

```
3 -5 0               ; signed values- 2-bit, 4-bit, and 0-bit widths
0x3 0xa2B1 0x0004    ; 2-bit, 16-bit, and 3-bit immediate values
```

***Labels***

Labels represent named addresses (resolved by assembling/linking). They consist of alpha-numeric characters (plus `_`), begining with a non-number character. Being addresses, labels always one word length (see ISA document PR2000). See below examples.

```
_Foo2 foobar L_0    ; valid labels
5_label             ; INVALID label

```

***Strings***

Strings, consisting of characters between `"` characters, represent an ordered, multi-byte array. Strings allow for some escape chars (listed below). Characters in strings are always stored in ascending order, left to right- occuying at least one byte each. See below examples.

```
\n \r \t \0 \" \\    ; valid escape chars within string

"string"             ; valid 6-character string
"escape \0"          ; valid 8-character string
""                   ; valid 0-character string
"not closed\"        ; INVALID string
```
*Note: Escape chars should be used instead of their real character counterparts- the latter having undefined behavior within strings.*

*Note: In asmld.exe's implementation, characters are packed 2 characters per word- the "first/lower" character occupying the lower value bits. e.g. "01" -> 0x3130. Appends 0x00 as needed for word alignment.*

***Arrays***

Arrays consist of 0+ immediates, labels, and strings between `{}` characters (delimited by whitespace). Array items are resolved (ie for size/value) individually prior to the array, which stores items in ascending order, left to right. See below examples.

```
{0x2 foobar "\"word\""}       ; 3 item array
{1 2 3 4 5 6 7 8 9 10}        ; 10 item array
{"hello" " " "world" "\0"}    ; 4 item array
```

### Statement Types

Assembly consists of different statements- each having a unique effect on the resulting output. Statements use variables, offering context to fully realize their meaning.

***Instructions***

Instructions are textual representations of the ISA's instruction set (see ISA document PR2000). Each instructions is translated into one ISA instruction in code memory (in the same order). See examples below.

```
SHR    $0 $3 12    ; Logical Shift Right $3 by 12, store in $0
SHR %a $0 $3 12    ; Arithmetic Shift Right $3 by 12, store in $0
JPR %r             ; Return From Interrupt
HLT                ; Halt
```

The table below summarizes all valid assembly instructions (in order of arguments- how arguments correspond to ISA instructions). Filled cells imply argument is required- except for "Flags", which are optional.

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

***Load Address Function***

The load address function is a "built-in" function for loading labels or word-sized immediates into a register. The function is always assembled as two LBI instructions back-to-back (ie code memory). See the example below.

```
_la    $3 0x8040    ; one load address function...

LBI    $3 0x80      ; ...equivalent two instructions
LBI %s $3 0x40

_la    $4 foo       ; if foo resolves to "0x02A1"...

LBI    $4 0x02      ; ...equivalent two instructions
LBI %s $4 0xA1
```

***Label Declarations***

Label declarations define new labels. Declared label represents the address of the next instruction, load address function, or (un)initialized data statement within the same file. See example below.

```
foo:       ; Label "foo" declared and paired with "HLT"
HLT

bar:       ; Label "bar" declared and paired with ".data 3"
boo:       ; Label "boo" declared and paired with ".data 3"
.data 3

```

***Label Modifiers***

Label modifiers add information about a label declared within the same file. Assembly supports modifiers `.global` and `.weak` (see [Linkage Rules](#linkage-rules) for more details). See example below.

```
.global foo    ; label "foo" modified as "global"

.global bar    ; label "bar" modified as "global" and "weak"
.weak   bar
```

***Initialized Data***

Initialized data is used to preset data memory (ie prior to code execution). Word-sized immediates, labels, strings, and arrays may be used as valid inputs to initialize data. See example below.

```
.data -5             ; word length immediate
.data foo            ; word length label
.data "Christmas"    ; 9 character string
.data {2 4 6 0 1}    ; 5 item array
```

***Uninitialized Data***

Uninitialized data is used to reserve data memory without requiring it to be a specific value. The amount of reserved data is specified in bytes- guaranteeing at least that many bytes are reserved. See example below.

```
.bss 20    ; 20 bytes reserved (with no starting value)

.bss 5     ; 5 bytes reserved (with no starting value)
```

*Note: In asmld.exe's implementation, uninitialized data reservations are padded as needed for word alignment.*

### Linkage Rules

Labels within an assembly program follow various rules when linking (ie pairing uses in load address functions and initialized data statements to label declarations). The following sections cover these rules.

***Label Defaults***

Upon declaration, a label is considered "local"- it can only be referenced within the same file. Futhermore, the declared label must be unique among
other locally declared labels. See example below.

```
myAsm.s
	foo:            ; declaration of "foo" label (tied to "HLT")
	HLT
	
	.data foo       ; reference to "foo" label above
	_la $0 bar      ; reference to "bar" label below
	
	bar: .bss 10    ; declaration of "bar" label (tied to ".bss 10")
	
	foo:            ; Illegal re-declaration of "foo" label above
```

*Note: Labels can be referenced/used prior to declaration, so long as they are eventually linked to a valid declaration.*

When linking labels and references, references will favor linking to local declarations over global ones. See example below.

```
file1.s
	foo:           ; declaration of "foo" label (tied to "NOP")
	NOP
	
	_la $0 foo     ; reference- links to file1.s "foo" declaration
	
file2.s
	.global foo
	foo:           ; global declaration of "foo" (ie visible to al files)
```

***Modifiers***

Modifiers alter the default behavior or declared labels.

The `.global` modifer allows a locally declared label to be visible to all files in the assembled program. The label declaration remains "local" within its own file, but becomes "global" in other files. See example below.

```
red.s
	.global foo
	foo:           ; global declaration of "foo" (tied to "HLT")
	HLT
	
blu.s
	_la $0 foo     ; reference- links to red.s "foo" declaration
```

*Note: Similar to local declarations, globalized labels must be unique among other globalized labels.*

The `.weak` modifier allows a locally declared label to become a secondary target for references- allowing local references to favor a global declaration over a local declaration. See example below.

```
weak.s
	.weak
	foo:           ; weak declaration of "foo"
	JPR %r
	
	.data foo      ; reference- links to glob.s "foo" declaration

glob.s
	.global foo
	foo:           ; global declaration of "foo"
```

***Special Labels***

Every assembly program must declare a `__START` label tied to an instruction or load address function statement. The label is used to specify the starting point of the program's code memory. See example below.

```
__START:    ; first instruction of the program is "NOP"
NOP
HLT
```

*Note: In asmld.exe's implementation, `__START` is automatically considered globalized and referenced. In effect, it can only be declared once and is never "unused".*

Assembly also pre-declares various labels (see table below). These labels are declared, referenced and globalized by default. They cannot be re-declared (even locally) and resolve to special values.

|Declared Label|Resolved Value                                             |
|--------------|-----------------------------------------------------------|
|\_\_BSS       |first address of uninitialized data memory                 |
|\_\_FREE      |first address of unused data memory                        |
|\_\_SIZE      |size of binary program/file (in bytes)                     |

### Miscellaneous

Comments can be added to assembly using the `;` character. All text between `;` and the next newline (or end of file) is considered purely decorative and has no effect on the program. See example below.

```
__START:    ; Comments can be made on the same line as a statement
HLT

; This comment has no effect

; The `;` prevents this from being a statement- ADD $4 $2 12
```

## Assembler/Linker Program
---
This section describes the asmld.exe program. It covers how to use the program with respect to its inputs and created output.

### Command Line Usage

The asmld.exe program is run by calling it via command line with various arguments. Arguments, regardless of order, may be flags (beginning with `-`) or input files. Below is the program's built-in help menu, summarizing its usage.

```
Assembler/Linker program for Project RISCII hardware
Usage: asmld.exe [options...] <input files...>

Options:
    -h        print this menu and exit
    -v        print version info and exit
    -o  <arg> sets filename to use for output file
    -ll <arg> sets stdout log level to:
                  silent  no log output
                  error   only errors
                  warning errors/warnings (default)
                  info    process related info
                  debug   all available output
    -g        adds debug symbols to binary image
    -r        recursively removes unused labels

```

At least one input file is required for creating an output file. Flags requiring an argument (e.g. `-o`) must provide a non-flag argument (if not a specific value). Other flags may forcibly exit. See example below.

```
> ./asmld                           // no input file(s)
requires at least one input file
> ./asmld -o -h                     // no "-o" argument
-o requires an argument
> ./asmld -f                        // bad flag
unrecognized '-f'
> ./asmld -ll foo                   // bad argument
unknown log level 'foo'
>./asmld -v -f                      // "-v" exits before handling bad flag
asmld.exe v62710f0
```

### Output File

A successful run of asmld.exe creates (or modifies) an output file with the assembled/compiled binary progam. The file is either named explicitly (via `-o`) or implicitly using one of the input files. See example below.

```
> ./asmld -o myImage a.s       // output name is explcitly given...
> ls
asmld.exe a.s myImage          // ...explicit output name used
> ./asmld a.s                  // no output name given...
> ls
asmld.exe a.bin a.s myImage    // ... ".bin" file made using input name
> ./asmld -o myImage a.s       // existing file specified...
> ls
asmld.exe a.bin a.s myImage    // existing file re-written
```

***File Organization***

The output file is formatted to the target RISCII hardware (see uP Manual Document PR2001). The list below describes where statements are placed in the file (if at all).

- **text section**: instruction, load address function
- **data section**: initialized data
- **neither**:      label declaration, label modifier, uninitialized data

Uninitialized data is not directly included in the file, but influences the binary program to "reserve" RAM memory inbetween the initialized data and unused memory. See illustration below.

```
|-------------| 0x0000 (start of data address space)
|.data values |
|-------------| __BSS
|.bss values  |
|-------------| __FREE
|unused memory|
|-------------| 0x8000
```

***Debug Effects***

Providing the `-g` option modifies the output file in two ways. First, it adds a "NOP" instruction directly at each globalized label located in the code memory. Second, it prepends a table tying each globalized label to its resolved address and output file section. See example below.

```
foo.s
	__START:             // "__START" label is global by default
	HLT
	
	foo: .data 4         // "foo" is local
	bar: .data 5         // "bar" is global
	
	.global bar

foo.bin
	__START,TEXT,0       // "__START" label in text section at address 0
	bar,DATA,2           // "bar" label in data section at address 2
	--DBG TABLE END--
	<binary program>     // binary program with "NOP" -> "HLT" instructions
```

The additional NOPs act as disabled breakpoints which can be located and "enabled" as HLT instructions using the table info and uP JTAG port (see uP Manual Document PR2001). The table also provides info to locate and read global data as necessary for debugging.

*Note: NOP instructions are added per label, not per address. If two labels address the same location, both will generate independent NOP breakpoints.*

***Removal Effects***

Providing the `-r` option removes unused labels and following related statements until a used label is found. This process is done individually per "region" of memory. Regions are defined by statements as follows:

- **code region**: instruction, load address function
- **data region**: initialized data
- **bss region**: uninitialized data
- **no region**: label declaration, label modifier

When statements are removed, references to other labels are undone, allowing other labels to become unused an thus removable. Removal is done recursively until no removals are made. See example below.

```
bar.s
	inner:              // used only by unused label's instructions
		NOP
		HLT
	
	myBss: .bss 4       // bss reservation referenced elsewhere in file
	
	used:               // code label referenced elsewhere in file
		JPR %r
		LBI $0 2
	
	outer:              // unused label- ripe for removal
		_la $1 inner    // reference in unused portion- to be undone
	
	myData: .data 4     // data reservation referenced elsewhere in file

bar.s (after -r)
	myBss: .bss 4       // bss reservation preserved
	
	used:               // referenced instructions preserved
		JPR %r
		LBI $0 2

	myData: .data 4     // data reservation preserved
```

Removal primarily acts as a way to remove unused data and functions from well-formatted assembly files. It does not guarantee complete dead code removal nor completely safe removal either (see below notes).

*Note: Label modifiers are applied prior to removal.*

*Note: If multiple labels tie to the same statement, the label just before the statement determines if the statement (and following statements) are removed.*

*Note: Removal is applied after conglomerating the program, meaning cross-file removal is possible, though not well defined or recommended.*

### Error Codes

Upon calling asmld.exe, a encoded value is returned indicating the result of the call. The table below describes each possible return code and its meaning.

|Value|Description                                                         |
|-----|--------------------------------------------------------------------|
|0x00 |successful run of asmld.exe; output file was created/updated        |
|0x01 |asmld.exe encountered a issue within its own design (ie a bug)      |
|0x02 |asmld.exe was unable to find or open a given file for reading       |
|0x03 |sequence of characters matched no known token in assembly           |
|0x04 |asmld.exe expected a specific assembly token, but received another  |
|0x05 |assembly token received was not within set of expected tokens       |
|0x06 |flag variable contained invalid flag- possibly invalid with context |
|0x07 |register variable was invalid or out of range)                      |
|0x08 |immediate value was invalid- possible invalid within context        |
|0x09 |generated output file is too large for target hardware              |
|0x0A |generated output file's RAM usage is too large for target hardware  |
|0x0B |asmld.exe was unable to create or open a given file for writing     |
|0x0C |label was declared locally more than once (ie within same file)     |
|0x0D |label declaration was created, but not tied to a statement          |
|0x0E |modifier given for label not declared at all or not in same file    |
|0x0F |label was declared globally more than once (ie in multiple files)   |
|0x10 |reference to label never defined (locally or globally)              |
|0x11 |required label `__START` was never declared                         |
|0x12 |required label `__START` was not tied to a "code memory" statement  |
|0x13 |string contained an unrecognized escape character                   |
|0x14 |command line argument flag requiring argument had no argument       |
|0x15 |no input files were passed to asmld.exe                             |
|0x16 |argument for a command line flag did not match any expected values  |
|0x17 |command line argument flag did not match any known flags            |

## Future Considerations
---
This sections describes potential features that can be added to the assembly language and/or asmld.exe program.

### Aggressive Warnings

At present, asmld.exe warns if a label if unused. This includes weakened labels whose references are (correctly) re-directed, this technically becoming unused (though the warning is unnecessary). See example below.

```
a.txt
	__START:
	_la $0 foo     // reference to "foo"- redirected to b.txt/2
	
	.weak foo
	foo:           // "Unused label 'foo'" despite intentionally unused
	NOP

b.txt
	.global foo
	foo:           // "foo" label referenced by a.txt/2
	JPR %r
```

Ideally, the warning should only appear for labels who were truly never referenced. If a label is weakened, it should only be warned about if the label was never referenced in the file (ie regardless of the modifer).

This change hasn't been implemented as it has limited impact at present. As more programs (compiler and source code) rely on asmld.exe, the change should be made accordingly.

### Improved Debug Symbols

The `-g` option currently only create table entries and breakpoints for globalized labels. It could technically create entries/breakpoints for any label. 

This change hasn't been implemented to limit bloat from debug breakpoints. As generated binary program sizes are more understood, the change may be made accordingly.

### Improved Label Removal

The algorithm for the `-r` option is, at present, simplistic. It is more striaghtforward, but also not as effective. Adding "section" modifiers, akin to gcc's assembler/linker, could improve asmld.exe's removal abilities.

This change hasn't been implemented yet for simplicity's sake. As the needs of making lean binary programs increases, the change may be made accordingly.

### Static Library Creation

Plans were made to add a "create static library" flag to asmld.exe. This flag would cause asmld to combine the assembly files into one assembly file (instead of creating a binary program).

This change hasn't been implemented for both simplicity and lack of use cases. As the need to create partially-compiled, single file libraries increases, the change should be made accordingly.