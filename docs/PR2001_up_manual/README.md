# PR2001: Microprocessor (uP) Manual
*"Softcore microprocessor implementing the RISCII ISA"*

**Purpose**

Define the workings and usage of the RISCII microprocessor, namely for use in physical setup and software programming.

**Overview**

> **TODO:** At present, this document covers details currently being developed/heavily worked on. Writings present are draft notes and thus may or may not reflect the final result.

**Common Terms**

|Term             |Description                                         |
|-----------------|----------------------------------------------------|
|uP               |RISCII microprocessor, the topic of this manual     |

**Base Documents**

|Document Number  |Description                                         |
|-----------------|----------------------------------------------------|
|PR2000           |RISCII instruction set architecture                 |

---

## Standards

The uP implements the RISCII ISA (see document PR2000).

## Output Binary Format

The created image created of aligned, big endian, 16-bit words. The image size can be between 4 and 32768 words (ie 8 bytes - 64 KB).

The image consists of two sections- text and data (in that order). Each section begins with a metadata word equal to the number of words in the section minus 1. Each section must have at least 1 word. See table below for unique fields in order of file appearance.

|Section Name  |Length (Words)|Description                                     |
|--------------|--------------|------------------------------------------------|
|.text metadata|1             |number of words in the following .text section  |
|.text value   |1+            |.text section of binary image                   |
|.data metadata|1             |number of words in the following .data section  |
|.data value   |1+            |.data section of binary image                   |
