# Project RISCII: Quaid (Compiler Interface) Manual (PR3003)
*"Reference for design of the RISCII compiler interface program"*

---

This document describes the requirements and usage of the RISCII Quaid Interface program. Contents also include developer's future considerations.

**Table of Contents**
- [Requirements](#Requirements)
- [Quaid Program](#quaid-program)
- [Future Considerations](#future-considerations)

**Referenced Terms**

|Term            |Description                                              |
|----------------|---------------------------------------------------------|
|cprep.exe       |c-preprocessor program (see Document PR3002)             |
|asmld.exe       |assembler/linker program (see Document PR3000)           |
|quaid.exe       |c-compiler interface program (topic of this document)    |


---

## Requirements
---
This section outlines the requirements of quaid.exe.

In short, quaid.exe expects to be in the same folder as programs cprep.exe and asmld.exe. These programs should be the programs discussed in documents PR3000-PR3002.

## Quaid Program
---
This section covers how to use quaid.exe.

### Command Line Usage

The quaid.exe program is called via the command line. Arguments are either flags (beginning with `-`) or input files. Below is the program's help menu.

```
Project RISCII Compiler (interface program)
Usage: quaid.exe [options...] <input files...>

Options:
    -h        print this menu and exit
    -v        print version info and exit
    -ll <arg> sets stdout log level to:
                  silent  no log info
                  error   only errors
              (d) warning errors and warnings
                  info    process related info
                  debug   all available output
    -as <arg> add assembly input file
    -E        pre-process only
    -I  <arg> add include directory path
    -D  <arg> define variable for all files
    -g        adds debug symbols to binary
    -o  <arg> sets binary filename
    -O1       do minor optimizations
```

At least one input file (directly or through the `-as` option) must be given. Flags requiring arguments must have a non-flag argument (if not a specific argument). See example below.

```
> ./quaid                           // no input files
requires at least one input file
> ./quaid -I                        // argument required
-I requires an argument
> ./quaid -ll foo                   // wrong argument given
unknown log level 'foo'
```

Arguments for `-I` and `-D` must follow formatting requirements. Repeating either in a call is allowed, but redundant. See example below.

```
> ls
quaid.exe foo.c ope/ ...         // (... among other files)
> ./quaid foo.c -I ./ope         // directory must end with '/'
Bad directory './ope'
> ./quaid foo.c -D 3D_GLASS      // variables cannot begin with number
Bad definition '3D_GLASS'
> ./quaid foo.c -D FOO -D FOO    // same as "./cprep foo.c -D FOO"
```

### Procedure

In general, quaid.exe calls the programs cprep.exe and asmld.exe in that order (feeding the outputs of one into the next).

The overall result is artifact files (ie `.i`) are produced, along with a binary image file (`.bin` or as named via the `-o` flag). See example below.

```
> ls
asmld.exe cprep.exe  foo.s  quaid.exe
> ./quaid.exe foo.c
> ls
asmld.exe  cprep.exe  foo.bin  foo.s  foo.i  quaid.exe

```

## Future Considerations
---
This section covers developer's thoughts about the program's (potential) future.

### Artifact Control

At present, quaid.exe (and its subprograms) reuse the same file paths for creating new files (except for the file extension). This tends to "clutter" the source directories.

In the future, having an option to direct artifacts to another directory may improve the program's quality of life.

### Error Count Reporting

At present, quaid.exe, when logging `info` level lines, only reporting on the number of errors/warnings from itself (vs including the counts from the subprograms).

In the future, finding a way to accumulate the errors within quaid.exe may improve the program's quality of life.

For now, having the subprograms report their own error/warning counts when setting quaid.exe to log `debug` level lines is acceptable.
