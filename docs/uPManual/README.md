# Project RISCII Microprocessor Manual
*"Reference manual for softcore microprocessor design"*

---

This README is a summary on how to use Project RISCII's (softcore) microprocessor design. These writings are primarly focused on the softcore uP design, though offer some insight into the desired FPGA/PCB implementation. This manual covers the following "internal" oriented material:
- uP Blocks
- uP State Machine
- Mapped Registers

Followed by the following "external" oriented material:
- uP Pinout
- "JTAG" Interface
- Binary Image Layout
- Integration Considerations

## uP Blocks
---
The uP design consists of many internal circuits and components. At a high level, the uP is made up of the following "blocks" of circuits:

|Block Name|Description                                                    |
|----------|---------------------------------------------------------------|
|JTAG      | JTAG port/interface for remote hardware access                |
|SCAN      | Read only boundary scan register accessible only through JTAG |
|CCTRL     | Mapped CORE registers and stack overflow detection controls   |
|BOOT      | Circuit to copy stored binary image to runtime memory on reset|

These blocks do not cover the entire uP design (ie smaller circuits exist to
route/synchronize signals), but reflect the primary "actors" and functions of the uP design at the high level.

## uP State Machine
---
Between various signals, the uP effectively implements a global "state machine" that dictates larger functionality (eg if firmware is running, which circuits control memory, etc). The high level states are as follows:

|State Name|SM Is Booted|SM Is Paused|Summary                              |
|----------|------------|------------|-------------------------------------|
|NOT_PAUSED|HIGH        |LOW         |uP is not doing anything of note     |
|PAUSED    |HIGH        |HIGH        |uP is "paused", JTAG now in charge   |
|BOOTING   |LOW         |LOW         |uP is preparing to run binary image  |

Pins "SM Is Booted" and "SM Is Paused" act as external indicators of the uP's internal state (see "uP Pinout" below for more details about the pins).

In the PAUSED state, uP's resources temporarily cease operation (eg firmware execution pauses, timers freeze, etc). In this mode, the JTAG is given general control over the uP's resources. Note that while the PAUSED state is not reached until all resources are paused, some resources may take longer to pause than others (ie frequent pausing may cause performance variation).

In the BOOTING state, the BOOT block is given control over both memory chips (other resources are put on hold). In this mode, the binary image is copied from the storage chip into the runtime chip (see section "Binary Image Format" below). This state cannot be interrupted, though eventually terminates and does not prevent uP resources from being paused (ie prevents formal recognition of uP being paused, but not pause process).

The NOT_PAUSED state is currently an **artifact of the uP being developed** at this time. It does not have any intended functionality other than being distinguished from the PAUSED state.

## Mapped Registers
---
Some of the uP's internal resources contain registers that can be accessed using specific addresses in the data address space (ie SW addresses 0x8000-0xFFFF, HW adddresses 0xC000-0xFFFF). The following subsections cover each register in detail, though a summary of each register is as follows:

|Register Name|SW Address|Access|Description                               |
|-------------|----------|------|------------------------------------------|
|CCTRL_CTRL   |0x8000    |r/w   |Controls for core processing features     |
|CCTRL_SETP   |0x8002    |r/w   |Setpoint for stack overflow detection     |

Unless otherwise stated, all registers are reset to a value of 0x0000 upon hardware reset. Likewise, any unspecified addresses are reserved registers with a read value of 0x0000.

### Core Controls (CCTRL) Registers

**_CCTRL_CTRL (SW Address = 0x8000, HW Address = 0xC000)_**

|Field Name|Bit Field|Description                                          |
|----------|---------|-----------------------------------------------------|
|reserved  |15:2     |reserved for future use- default value(s) = 0        |
|ovdEn     |1        |overflow detection enable                            |
|doPause   |0        |pauses uP (automatically set by HLT instruction)     |

At present, "ovdEn" and "doPause" have no effect **(ie artifact of uP currently under development)**.

**_CCTRL_SETP (SW Address = 0x8002, HW Address = 0xC001)_**

|Field Name|Bit Field|Description                                          |
|----------|---------|-----------------------------------------------------|
|setpoint  |15:0     |lowest valid stack pointer (ie "less than" trigger)  |

At present, "setpoint" has no effect **(ie artifact of uP currently under development)**.

## uP Pinout
---
The uP interfaces with the physical FPGA (and by extension outside world) using various "pin" (verilog port) connections. These pins are used to provide the uP with clock/reset signals, volatile/non-volatile memory, and various means of communication. Below is a summary of each pin group:

|Pin Group Name|# of Pins|Direction|Summary                               |
|--------------|---------|---------|--------------------------------------|
|MEM Address   |16       |Output   |16-bit word address to runtime chip   |
|MEM Write     |1        |Output   |read/write runtime chip specifier     |
|MEM Enable    |1        |Output   |enable to trigger runtime chip access |
|MEM Data      |16       |Inout    |16-bit runtime chip data (read/write) |
|SPI MISO      |1        |Input    |input data coming from storage chip   |
|SPI MOSI      |1        |Output   |output data going to storage chip     |
|SPI CLK       |1        |Output   |clock synching storage chip access    |
|SPI CSn       |1        |Output   |enable to trigger storage chip access |
|GPIO          |16       |Inout    |generic digital pins for software use |
|JTAG TCK      |1        |Input    |clock synching jtag access            |
|JTAG TMS      |1        |Input    |select signal controlling jtag state  |
|JTAG TDI      |1        |Input    |input data entering jtag              |
|JTAG TDO      |1        |Output   |output data exiting jtag              |
|SM Do Pause   |1        |Input    |external input to pause uP            |
|SM Is Booted  |1        |Output   |indicator output that uP is booted    |
|SM Is Paused  |1        |Output   |indicator output that uP is paused    |
|SYS Clock     |1        |Input    |core clock frequency used by uP       |
|SYS Reset     |1        |Input    |external input to reset uP hardware   |

All pins are digital and should only ever be set to logic HIGH or LOW (except for bidirectional/"Inout" pins, which may float at high impedance "Z").

For info about integrating these pins with the FPGA/board design, see the "Integration Considerations" section below.

## "JTAG" Interface
---
The JTAG interface is the uP's "maintenance hatch", allowing hardware access and control to an external host. It is intended to provide, among other features: partial boundary scan (ie hardware debugging), programming, and software debugging.

### Basic Usage

Similar, though not compliant, to the formal JTAG standard, the interface consists of 4 pins, transfering data between the uP and host. The pins themselves are used similar to SPI (CPOL/CPHA = 0) pins, except the select pin is used to navigate a state machine (instead of being a simple on/off switch). The uP JTAG state machine is as follows:

|Curent State|Next State on TCK Posedge|Current State Summary              |
|------------|-------------------------|-----------------------------------|
|IDLE        |TMS == 0 ? ISEL : IDLE   |jtag not in use- idling            |
|ISEL        |TMS == 0 ? ISHFT : DSEL  |"instruction/command" selected     |
|DSEL        |TMS == 0 ? DSHFT : IDLE  |"data/value" selected              |
|ISHFT       |TMS == 0 ? ISHFT : UPDATE|shifting in instruction/command    |
|DSHFT       |TMS == 0 ? DSHFT : IDLE  |shifting in data/value             |
|UPDATE      |IDLE                     |executing instruction/command      |

The interface allows for left shifting (ie MSb first) either an 8-bit command or 16-bit data value in/out of the uP (states ISHFT and DSHFT, respectively). Data values are saved/stored until next access, while commands will attempt to be executed once they are done shifting in.

Additionally, when shifting in a command, an 8-bit status byte will be shifted out first. This byte contain information about the uP as described below:

|Field Name|Bit Field|Description                                          |
|----------|---------|-----------------------------------------------------|
|reserved  |7:2      |reserved for future use- default value(s) = 0        |
|isPaused  |1        |set if uP hardware is currently paused               |
|isBooted  |0        |set if uP hardware has finished booting              |

### Commands

Below is a table of the JTAG's supported commands:

|Command Name|Command Code|Requirements|Desciption                         |
|------------|------------|------------|-----------------------------------|
|No Operation|0x00        |None        |No action taken- "get status" cmd  |
|Set MEM Addr|0x01        |None        |Saves data register as MEM address |
|MEM Read    |0x02        |Paused      |Reads MEM Address to data register |
|MEM Write   |0x03        |Paused      |Write data register to MEM address |
|Access SCAN |0x04        |Paused      |Map SCAN register to data register |
|Access SPI  |0x05        |Paused      |Map SPI storage to data register   |
|Run uP      |0x06        |Booted      |De-asserts JTAG's pause request    |
|Pause uP    |0x07        |Booted      |Asserts JTAG's pause request       |

Note that some commands have requirements. These requirements center around the uP's current overall state (i.e. BOOTING, RUNNING, or PAUSED). If the uP state does not meet the requirements, the commands is not executed.

For MEM read/write (ie runtime chip accesses), it is worth noting the 16-bit address provided is word addressable. That is, addresses 0x0000 and 0x0001 access completely different bits/bytes/words in the memory. With respect to the application software, addresses 0x0000-0x7FFF correspond to the program address space while addresses 0x8000-0xFFFF correspond to the data address space.

For SCAN/SPI accesses, it is worth noting that the "masking/mapping" of each device only lasts until another command is given (even if the given command is inavlid/has no effect).

For uP pause commands, the commands only control the JTAG's pause request- not other sources. That said, other sources (except for the "SM Do Pause Pin") are typically memory mapped and can be overwritten by the JTAG's MEM write command.

### Boundary Scan Register

Similar to the formal JTAG standard, the JTAG interface implements a special boundary scan register to access the uP's raw pins (albeit in a read only mode). The register acts as one larger shift register, with following bit interpretation/behavior:

|Field Name|Bit Field|Description                                          |
|----------|---------|-----------------------------------------------------|
|reserved  |63:57    |reserved for future use- default value(s) = 0        |
|smIsPaused|56       |indicator output that uP is paused                   |
|smIsBooted|55       |indicator output that uP is booted                   |
|smDoPause |54       |external input to pause uP                           |
|gpioPins  |53:38    |generic digital pins for software use                |
|spiCSn    |37       |enable to trigger storage chip access                |
|spiCLK    |36       |clock synching storage chip access                   |
|spiMOSI   |35       |output data going to storage chip                    |
|spiMISO   |34       |input data coming from storage chip                  |
|memEn     |33       |enable to trigger runtime chip access                |
|memWr     |32       |read/write runtime chip specifier                    |
|memData   |31:16    |16-bit runtime chip data (read/write)                |
|memAddr   |15:0     |16-bit word address to runtime chip                  |

## Binary Image Format
---
The uP is designed to store a binary image (up to 64 KB in size) on the storage memory chip between power cycles. Once powered on, it reads the binary image into the runtime chip for execution. To properly facilitate this process, the binary image shall be formatted as follows:

|Section Name  |Length (Bytes)|Description                                 |
|--------------|--------------|--------------------------------------------|
|.text metadata|2             |Last valid .text HW address                 |
|.text section |2+            |.text section of program                    |
|.data metadata|2             |Last valid .data HW address                 |
|.data section |2+            |.data section of program                    |

Each section of the software is preceded by a word of metadata describing the last HW address used to store the following section (.text implicitly starting from HW address 0x0000, .data from HW address 0x8000). Neither metadata word is copied to the runtime chip.

The .text section is where the program's instructions are stored while the .data section is where any global initialized program data is stored. Both are better described in documentation about the Instruction Set Architecture (ISA) and Assembler/Linker (asmld) Manuals. Importantly, though, they use SW addresses to refer to the runtime chip instead of HW addresses.

In general, all values in the binary image are, at most precise, 2-byte words. As applicable, big endian (ie MSB first) applies within each word.

## Integration Considerations
---
As stated before, this document focuses on describing the software uP design, NOT the the physical, FPGA implemented feature. However, to ensure proper implementation, the following sections cover FPGA related details the softcore uP design expects to be handled.

### Additional Interface Controls

Interfaced chips (eg memory chips) may have more control signals than the uP design specifies. Additional controls should be tied to power/ground rails or handled using a "wrapper module" on the FPGA (see subsection "Wrapper Module"). The uP design should have sufficent controls for most interfaced chips.

Furthermore, specific interface requirements (eg timing, command codes, etc) will need to be considered while integrating chips with the uP. At prsent, Most assumptions made by the uP (beyond inferred use of pinouts) is the SPI interface with the storage chip, which must meet the following criteria:
- SPI mode 0 (CPOL/CPHA = 0), MSb first
- 8-bit command codes, 16-bit addresses
- "read bitstream" command with sequence "command","address","read out"
- "read bitstream" command of "0x03" (must change design based on chip)

As stated, the exact command byte used to trigger a read is hardwired into the design (in file "processor/DevProject/src/ProcBlocks/BootImage.v") and must be changed based on the exact storage chip used.

Lastly, while unlikely, be aware of niche timing considerations (eg hold times on falling edges, specific access times, etc).

### Wrapper module

For better FPGA/uP integration, it is recommended a "wrapper module" be made per specific FPGA used. This module should instantiate the uP design, then route the physical FPGA pins to the uP's "pins", thus integrating the design without changing its source logic.

A wrapper module may also be needed for pins that exist on both the uP and external elements, but have different active edges. See subsection "Pin Pulling" for more details on pins where active edges/levels are cruical.

### Pin Pulling

For safety/reliability, some pins are expected to explicitly have a pullup or pulldown resistor. Below summarizes these pins and their reasons for desiring a pull:

|Pin Group Name|Expected Pull|Reason                                       |
|--------------|-------------|---------------------------------------------|
|JTAG TCK      |down/LOW     |prevent errant clock ticks on connection     |
|JTAG TMS      |up/HIGH      |prevent errant state traversal on connection |
|SYS Reset     |up/HIGH      |keep active-low signal "off" when not in use |
|MEM Enable    |down/LOW     |prevent errant memory accesses               |
|SPI CLK       |down/LOW     |prevent errant storage accesses              |
|SPI CSn       |up/HIGH      |prevent errant storage accesses              |
