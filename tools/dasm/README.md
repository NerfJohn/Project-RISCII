# RISCII Disassembler (dasm)
*"Technical notes for development and design"*

**Purpose**

Informally track "cliff notes" regarding tool's design.

**Overview**

> **NOTE**: This is an informal document meant to keep track of design notes for latet development/maintenance. Thus, this document is especially prone to clumsy pacing and rushed notes.

**Common Terms**

|Term            |Description                                     |
|----------------|------------------------------------------------|
|---             |---                                             |

---

## User Story

This tool is meant to solve a few issues:

1. General insurance policy for future debugging (i.e. if stuff gets REALLY bad and binaries are in question)
2. A more direct way to test the asmld.exe program during development

## Approach

**Values**

Image effectively a bunch of 16-bit values. Will want be read them in various ways:

- as integer: either to get metadata count or for easier bit masking
- as hex: likely best way to print out

**Decoding**

There's a limited number of 'patterns' for instructions:

- Std data operations (opcd-dst-sr1-000-sr2 and opcd-dst-sr1-1-5bimm)
    - JPR and JLR use the immediate version
- SHR operation (opcd-dst-sr1-0-a-0-sr2 and opcd-dst-sr1-1-a-4bim)
- LBI operation (opcd-dst-s-8bimmval)
- memory operations (opcd-dst-sr1-6bofst)
- BRC operation (opcd-flgs-8bimmval)
- command operation (opcd-set vals)

Narrowing down the pattern tends to be a 1) determine the opcode then 2) check or a certain flag

## General Features

Hoping for it to generally work as such:

- Basic:      convert binary into single file assembly- simple instructions + .data arrays of signed ints
- Helpful:    prepend address indicators to file to ease determining how stuff lays out in memory
- Critical:   indicates if instruction included additional "flags" (ie benign non-zeroed bits)
- Analytics:  include (via comments) general observations (size per section, type distribution, etc)

Data is generally put into created output file, though would be nice if it could be printed to stdout (ie analytics + critical issues, not actul resolutions).