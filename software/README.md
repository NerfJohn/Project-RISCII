# Software: Single file C Compiler
This folder contains the software's design, testing scripts, and tests. Below gives a summary of software use/navigation/limitations.

## Class Diagram of Source Code
![sw_top_level.jpg](https://github.com/NerfJohn/Project-RISCII/blob/main/software/sw_top_level.jpg)
(Simplified version of classes in [src](https://github.com/NerfJohn/Project-RISCII/blob/main/hardware/proc/proc.v) directory.)

## Folder Stucture
- /src: Software design files (denoted by .cpp and .h files). Note that some are generated from mock-ups under [/scripts](https://github.com/NerfJohn/Project-RISCII/blob/main/software/scripts/).
- /scripts: Mock-ups, generators, and tools used to build and debug the source code.
- /tests: Software tests (written in C- see [Requirements.txt](https://github.com/NerfJohn/Project-RISCII/blob/main/docs/Requirements.txt) for syntax).

## Top Level Scripts
- /runBuild.sh: Builds the compiler from source code *(./runBuild.sh [-h] [-rebuild])*
  - Builds the compiler from the source code under [/src](https://github.com/NerfJohn/Project-RISCII/blob/main/software/src/)
  - Will implicitly rebuild if source code (or mock-ups) are updated)
- /runTests.sh: Runs batch of tests *(./runTests.sh [-h] \<directory\>)*
  - Takes input directory to scan for tests (denoted by .c files).
  - Exact passing requirements depend on type of test (denoted by sub-folder under [/tests](https://github.com/NerfJohn/Project-RISCII/blob/main/software/tests/)
  - Some tests may run a produced .hex file on the hardware design by calling [/runSim.sh](https://github.com/NerfJohn/Project-RISCII/blob/main/hardware/runSim.sh)

## Testing Solution/Strategy
- Testing designed around checking C file is functionally interpreted correctly.
- Automatic testing supported by tests compiling and returning 1 from main() when run on the hardware.
- Specific/automatic software testing is limited with the current solution.
  - Internal/assembly level implementation of code is not explicitly checked.
  - The optimization feature of the compiler is not explicitly checked (beyond it affecting functionality).

## Toolchain Considerations
Software design/tools/tests are known to use the following tools:
- g++ (for compiling the compiler's source code into an executable)
- Git Bash (for running bash scripts on a Windows machine)
- Python (for running various scripts, namely generators to generate some compiler source code)
- (Test suite makes use of hardware simulation- hardware toolchain requirements apply)

## Extra Documents
- [ISA.txt](https://github.com/NerfJohn/Project-RISCII/blob/main/docs/ISA.txt)
- [Requirements.txt](https://github.com/NerfJohn/Project-RISCII/blob/main/docs/Requirements.txt)
- [SWTestPlan.txt](https://github.com/NerfJohn/Project-RISCII/blob/main/docs/SWDesignPlan.txt)
- [SWDesignPlan.txt](https://github.com/NerfJohn/Project-RISCII/blob/main/docs/SWTestPlan.txt)
- [swRetrospective.txt](https://github.com/NerfJohn/Project-RISCII/blob/main/docs/swRetrospective.txt)
