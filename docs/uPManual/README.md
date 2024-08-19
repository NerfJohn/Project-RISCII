# Project RISCII Microprocessor Manual
*"Reference manual for softcore microprocessor design"*

---

This README is a summary on how to use Project RISCII's (softcore) microprocessor design. At present, these writings refer specifically to the softcore implementation (ie they do not consider details about the FPGA implementing the design). This manual covers the following material:
- uP Pinout
- "JTAG" Interface
- Integration Considerations

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

Note that some commands have requirements. These requirements center around the uP's current overall state (i.e. BOOTING, RUNNING, or PAUSED). If the uP state does not meet the requirements, the commands is not executed.

For MEM read/write (ie runtime chip accesses), it is worth noting the 16-bit address provided is word addressable. That is, addresses 0x0000 and 0x0001 access completely different bits/bytes/words in the memory. With respect to the application software, addresses 0x0000-0x7FFF correspond to the program address space while addresses 0x8000-0xFFFF correspond to the data address space.

For SCAN/SPI accesses, it is worth noting that the "masking/mapping" of each device only lasts until another command is given (even if the given command is inavlid/has no effect).

## Integration Considerations
---

As stated before, this document focuses on describing the software uP design, NOT the the physical, FPGA implemented feature. However, to ensure proper implementation, the following sections cover FPGA related details the softcore uP design expects to be handled.

### Wrapper module

For better FPGA/uP integration, it is recommended a "wrapper module" be made per specific FPGA used. This module should instantiate the uP design, then route the physical FPGA pins to the uP's "pins", thus integrating the design without changing its source logic.

### Additional Controls

Interfaced chips (eg memory chips) may have more control signals than the uP design specifies. Additional controls should be tied to power/ground rails or handled using a "wrapper module" on the FPGA. The uP design should have sufficent controls for most interfaced chips.

Furthermore, specific interface requirements (eg commands for the SPI storage chip) will need to be considered while integrating chips with the uP. At present, the uP makes no assumptions on these interfaces beyond its pinout and inferred use (eg the "MEM Address" pins are used to give an address, etc).

### Pin Pulling

For safety/reliability, some pins are expected to explicitly have a pullup or pulldown resistor. Below summarizes these pins and their expectations:

|Pin Group Name|Expected Pull|Reason                                       |
|--------------|-------------|---------------------------------------------|
|JTAG TCK      |down/LOW     |prevent errant clock ticks (due to noise)    |
|JTAG TMS      |up/HIGH      |prevent errant state traversal (due to noise)|
|SYS Reset     |up/HIGH      |keep active-low signal "off" when not in use |
