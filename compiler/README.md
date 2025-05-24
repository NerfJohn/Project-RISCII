# Compiler
*"A Convenient Bit Translator"*

---

This folder contains files/folders related to Project RISCII's compiler. **Compilers** translate human readable code into computer readable code. This compiler is run as a collection of separately compiled sub-programs.

#### Folders

|Folder Name|Description                                                   |
|-----------|--------------------------------------------------------------|
|\_shared   |resourced/scripts associated with the compiler at large       |
|asmld      |development items for compiler's assembler/linker program     |
|cprep      |development items for compiler's c-preprocessor program       |
|quaid      |development items for compiler's interface program            |

#### Files

|File   Name|Description                                                   |
|-----------|--------------------------------------------------------------|
|README.md  |this file- describes folder structure and usage               |

## Usage

To compile the RISCII Compiler, see and call `./_shared/genBins.sh` in bash. This will create a new folder `./_shared/bin` with the compiled sub-programs.

Once compiled, the RISCII compiler can be invoked using `./_shared/bin/quaid.exe`. See the following documents (under `<project root>/docs`) for more help.

- QuaidManual (PR3003)
- CprepManual (PR3002)
- AsmldManual (PR3000)

## Software Requirements

The RISCII compiler was developed on a Windows 11 machine with the following software:

- **MinGW GCC/G++** v9.2.0 (for compiling sub-programs)
- **CMake** v3.30.0-rc3 (for compiling sub-programs)
- **Git Bash** v5.1.16 (for running build/helper scripts)
- **Git** v2.38.1 (for version control)
- **Eclipse IDE** v4.27.0 2023-03 (for code development)
