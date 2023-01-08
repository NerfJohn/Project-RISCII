# Hardware: Processor & L1 Caches
This folder contains the hardware's design, simulation/testing scripts, and tests. Below gives a summary of hardware use/navigation/limitations.

## Diagram of Top Level Design
![hw_top_level.jpg](https://github.com/NerfJohn/Project-RISCII/blob/main/hardware/hw_top_level.jpg)
(Simplified version of [proc.v](https://github.com/NerfJohn/Project-RISCII/blob/main/hardware/proc/proc.v).)

## Folder Stucture
- /proc: Hardware design files (denoted by .v files).
- /scripts: Simulation/testing scripts used to run the simulation and assemble tests.
- /tests: Hardware tests (written in assembly- see [ISA.txt](https://github.com/NerfJohn/Project-RISCII/blob/main/docs/ISA.txt) for syntax).

## Top Level Scripts
- /runSim.sh: Runs a simulation of hardware *(./runSim.sh [-h] <input .hex file> [-test])*
  - Input .hex file used to set ROM (unspecified bytes set to 0).
  - Creates .out file if exit code == 0 (.out contains first 2-bytes of cache and clk cycle count, respectively).
  - test flag enables extra requirements (namely a 500 clk cycle timeout failure).
- /runTests.sh Runs batch of tests *(./runTests.sh [-h] <directory>)*
  - Takes input directory to scan for tests (denoted by .asm files).
  - Test must assemble, simulate, and report "1" as its cache value to pass.

## Testing Solution/Strategy
- Testing designed around checking ISA is implemented correctly.
- Automatic instruction testing supported by tests setting cache value to 1 if successful.
- Specific/automatic hardware testing is limited with the current solution.
  - Halt signal explicitly tested in terms of timing/preventing cache writing.
  - Caching/pipelining implicitly semi-tested via automated tests (see /tests/hw_tests).

## Toolchain Considerations
Hardware design/tools/tests are known to use the following tools:
- ModelSim (for compiling/simulating the hardware)
- Git Bash (for running bash scripts on a Windows machine)
- Python (for running the hardware test assembler)
