# Project RISCII: uP (Microprocessor) Manual (PR2001)
*"Reference for design of the RISCII softcore microprocessor"*

---

This document describes the capabilities and functions of the RISCII softcore processor (64 KB NVM/32 KB RAM, rated for 8 MHz system clock). Includes pertient details for software usage and hardware setup.

**Table of Contents** 

- [Pinout](#pinout)
- [Block Diagram](#block-diagram)
- [High Level Operation](#high-level-operation)
- [JTAG Interface](#jtag-interface)
- [Binary Image Format](#binary-image-format)
- [Mapped Registers](#mapped-registers)
- [Integration Considerations](#integration-considerations)

**Referenced Terms**

|Term            |Description                                              |
|----------------|---------------------------------------------------------|
|ISA             |"ISA" Document PR2000                                    |
|uP              |RISCII softcore microprocessor                           |
|KB              |kilobyte, = 1024 bytes                                   |
|MHz             |megahertz, = 1,000,000 hertz                             |
|RAM             |SRAM runtime memory chip, external to uP                 |
|NVM             |EEPROM storage memory chip, external to uP               |
|IO              |input/output                                             |
|ext             |external                                                 |
|JTAG            |RISCII's version of common hardware std of the same name |
|CLK             |clock, system clock unless specified (eg NVM CLK)        |
|SYS             |system, with respect to uP at large                      |


---

## Pinout
---
This section outlines the uP's pinout (ie module IO of the softcore uP).

The table below summarizes the uP's critical pinout (VDD/GND not included).

|Pin Group Name|# of Pins|Direction|Summary                                |
|--------------|---------|---------|---------------------------------------|
|RAM Address   |16       |Output   |RAM access word address                |
|RAM Write     |1        |Output   |RAM access write/read signal (0 = rd)  |
|RAM Enable    |1        |Output   |RAM access enable signal (0 = off)     |
|RAM Data      |16       |Bidir    |RAM access data                        |
|NVM MISO      |1        |Input    |NVM access data ouput to uP            |
|NVM MOSI      |1        |Output   |NVM access data output to NVM          |
|NVM CLK       |1        |Output   |NVM access shared clock                |
|NVM CSn       |1        |Output   |NVM access enable signal (0 = on)      |
|GPIO Pins     |16       |Bidir    |generic digital IO pins                |
|JTAG TCK      |1        |Input    |JTAG access shared clock               |
|JTAG TMS      |1        |Input    |JTAG access state select signal        |
|JTAG TDI      |1        |Input    |JTAG access data input                 |
|JTAG TDO      |1        |Output   |JTAG access data output                |
|SM Do Pause   |1        |Input    |external pause signal (0 = off)        |
|SM Is Booted  |1        |Output   |indicator uP has exited BOOTING state  |
|SM Is Paused  |1        |Output   |indicator uP is in PAUSED state        |
|SYS Clock     |1        |Input    |system clock of uP                     |
|SYS Reset     |1        |Input    |external reset signal (0 = on)         |

## Block Diagram
---
This section provides an overview of the major circuit blocks of the uP. It covers internal and external circuits.

The table below describes circuit blocks internal to (thus provided by) the uP.

|Block Name|Description                                                    |
|----------|---------------------------------------------------------------|
|JTAG      |JTAG-esque port capable of programming and debugging           |
|SCAN      |Read-only boundary scan register, extension of JTAG block      |
|BOOT      |Hardware bootloader, reads binary image from storage chip      |
|CORE      |Core processor responsible for code execution                  |
|CCTRL     |Mapped CORE registers and stack overflow detection controls    |
|NVIC      |Mapped interrupt controller with 11 unique types/sources       |
|WDT       |Mapped watchdog timer capable of causing hardware reset        |
|GPIO      |Mapped generic digital IO pins, some with alternate functions  |
|TMR0      |Mapped generic configurable timer                              |
|TMR1      |Mapped generic configurable timer                              |
|TMR2      |Mapped configurable timer with PWM capabilities                |
|TMR3      |Mapped configurable timer with PWM capabilities                |
|UART      |Mapped UART transceiver with configurable baud rate            |
|I2C       |Mapped master only I2C with configurable baud rate             |
|SPI       |Mapped master only SPI hardwired to the storage chip           |

By contrast, the following table (below) describes circuit blocks the uP expects to be provided with externally.

|Block Name|Description                                                    |
|----------|---------------------------------------------------------------|
|CLK       |system clock, std square wave signal at a constant frequency   |
|RAM       |SRAM runtime chip, at least 96 KB and parallel port accessed   |
|NVM       |EEPROM storage chip, at least 64 KB and SPI port accessed      |

## High Level Operation
---
This section describes the high level 1) state machine 2) memory layout and 3) Core Architecture of the uP.

### State Machine

The uP has three over-arching states that correspond to who controls the memory chips (ie runtime/RAM and storage/NVM). The table below summarizes these states.

|State Name|Memory Chips Controller|Description                            |
|----------|-----------------------|---------------------------------------|
|BOOTING   |BOOT                   |uP is reading binary image into RAM    |
|RUNNING   |CORE/Mapped Registers  |uP is executing code (out of RAM)      |
|PAUSED    |JTAG                   |uP is on standby for JTAG access       |

Upon a hardware reset, the uP begins in the BOOTING state, which always terminates. Depending on the various pause signals, the uP will continue in the RUNNING or PAUSED states, transitioning as instructed.

For RUNNING/PAUSED transitions, while only one pause signal is needed to begin a pause, all uP circuits must fully pause before becoming PAUSED (see [Mapped Registers](#mapped-registers)). The table below summarizes the sources of pause signals.

|Signal Name|Signal Origin|Description                                     |
|-----------|-------------|------------------------------------------------|
|Ext Pause  |SM Do Pause  |uP pin providing external method of pausing     |
|JTAG Pause |JTAG         |JTAG setting allowing for "ext host" pausing    |
|CCTRL Pause|Mapped CCTRL |mapped bit used by CORE for software pausing    |

*Note: PAUSED state implements ISA's "paused mode".*

### Memory Layout

Memory is implemented between 1) the runtime chip 2) the uP's mapped registers and 3) the storage chip. How these regions are used vary based on the "level" they are viewed at.

**_Hardware Level View_**

Interal hardware, such as BOOT and JTAG, view memory for what it is. The table below summarizes how they view memory.

|Section Name|Address Range|Bits Per Address|Address Space|Summary         |
|------------|-------------|----------------|-------------|----------------|
|RAM         |0x0000-0xBFFF|16              |Memory       |runtime chip    |
|Mapped      |0xC000-0xFFFF|16              |Memory       |mapped registers|
|NVM         |0x0000-0xFFFF|8               |Storage      |storage chip    |

**_Software Level View_**

Software, executed by CORE, views memory in an abstract manner. The table below summarizes how software views memory.

|Section Name|Address Range|Bits Per Address|Address Space|Summary         |
|------------|-------------|----------------|-------------|----------------|
|Progam      |0x0000-0xFFFF|8               |Program      |text section    |
|Data        |0x0000-0x7FFF|8               |Data         |data/open memory|
|Mapped      |0x8000-0xFFFF|8               |Data         |mapped registers|

Software sees two separate address spaces (within RAM), where one space is partially mapped to internal registers. Address spaces appear byte addressable, though as seen in the hardware view, are actually addressed by word.

### Core Architecture

The uP's CORE complies with the ISA given in PR2000. In terms of "interrupt/application" modes, the CORE supports interrupts and defines a unique mode in which it executes interrupt related code.

Beyond ISA requirements, the CORE is a (roughly) 2 stage, Harvard style architecture (albeit a single, shared memory pipeline). As described in the above memory layout, it strictly uses RAM for executing a binary image.

## JTAG Interface
---
This section describes the uP's JTAG interface. It covers 1) pins and state machine 2) JTAG commands and ) additional features.

*Note: As stated above, the uP's JTAG does not completely follow the JTAG std common to most modern microcontrollers.*

### Pin and State Machine

The JTAG uses 4 pins in a similar manner to SPI (ie full duplex, clocked with select signal). Following the comparison, the JTAG uses CPOL/CPHA = 0 and MSb semantics. See [Pinout](#pinout) for more details on JTAG pins.

Unlike SPI, the select signal (ie JTAG TMS) is used to traverse a state machine. The table below summarizes the state machine.

|Curent State|Next State on TCK Posedge|Current State Summary              |
|------------|-------------------------|-----------------------------------|
|IDLE        |TMS == 0 ? CSEL  : IDLE  |JTAG not in use- idling            |
|CSEL        |TMS == 0 ? CSHFT : DSEL  |"command" selected                 |
|DSEL        |TMS == 0 ? DSHFT : IDLE  |"data" selected                    |
|CSHFT       |TMS == 0 ? CSHFT : UPDATE|shifting in 8-bit command          |
|DSHFT       |TMS == 0 ? DSHFT : IDLE  |shifting in 16-bit data            |
|UPDATE      |IDLE                     |executing command                  |

### JTAG Commands

Values can be shifted in/out of either an 8-bit command register or 16-bit data register. After shifting in a command, it is executed in the next TCK cycle. The table below summarizes the available JTAG commands.

|Command Name|Command Code|Pre-Reqs|Summary                                |
|------------|------------|--------|---------------------------------------|
|No Operation|0x00        |None    |No operation performed                 |
|RAM Addr    |0x01        |None    |Sets address used for RAM commands     |
|RAM Read    |0x02        |Paused  |Reads word from RAM into data register |
|RAM Write   |0x03        |Paused  |Writes word to RAM from data register  |
|Access SCAN |0x04        |Paused  |Maps SCAN register over data register  |
|Access NVM  |0x05        |Paused  |Maps NVM connection over data register |
|Run uP      |0x06        |Booted  |De-asserts JTAG's pause signal         |
|Pause uP    |0x07        |Booted  |Asserts JTAG's pause signal            |

Additional notes regarding JTAG commands are as follows:
- Commands that are unrecocgnized or don't meet pre-reqs are ignored
- The JTAG uses hardware level memory (see [High Level Operation](#high-level-operation))
- SCAN/NVM accesses remain active until the next command is executed
- On a hardware reset, SCAN/NVM accesses and the pause signal are inactive

### Additional Features

**_Status Code_**

When the command register is shifted, an 8-bit status code is initially shifted out. The table below summarizes the meaning of each bit (field).

|Field Name|Bit Field|Description                                          |
|----------|---------|-----------------------------------------------------|
|reserved  |7:2      |reserved for future use (value = 0)                  |
|isPaused  |1        |set if uP is in PAUSED state                         |
|isBooted  |0        |set if uP has exited the BOOTING state               |

*Note: Recommended to use "No Operation" command to extract status.*

**_Boundary Scan (SCAN) Register_**

The SCAN register provides read only access to the uP external pins (excluding SYS and JTAG pins). It is accessed via shifting (sampling all other times). The table below summarizes the meaning of each bit (field).

|Field Name|Bit Field|Description                                          |
|----------|---------|-----------------------------------------------------|
|reserved  |63:57    |reserved for future use (value = 0)                  |
|smIsPaused|56       |indicator uP is in PAUSED state                      |
|smIsBooted|55       |indicator uP has exited BOOTING state                |
|smDoPause |54       |external paused signal                               |
|gpioPins  |53:38    |generic digital IO pins                              |
|nvmCSn    |37       |NVM access enable signal                             |
|nvmCLK    |36       |NVM access shared clock                              |
|nvmMOSI   |35       |NVM access data output to NVM                        |
|nvmMISO   |34       |NVM access data output to uP                         |
|ramEn     |33       |RAM access enable signal                             |
|ramWr     |32       |RAM access write/read signal                         |
|ramData   |31:16    |RAM access data                                      |
|ramAddr   |15:0     |RAM access word address                              |

## Binary Image Format
---
This section describes how the uP interprets the binary image stored in NVM (thus, how the image should be formatted). It covers the layout of the image file and how it is copied into RAM.

### Image File Layout

The image file is divided into 4 distinct sections. It is also word aligned, big endian, and totalling no more than 64 KB. The table below summarizes each section (in ascending address order).

|Section Name  |Length (Bytes)|Description                                 |
|--------------|--------------|--------------------------------------------|
|.text metadata|2             |last .text address in RAM (hardware level)  |
|.text value   |2+            |.text section of binary image               |
|.data metadata|2             |last .data address in RAM (hardware level)  |
|.data value   |2+            |.data section of binary image               |

A program is directly made up of instructions (.text) and pre-initialized data (.data). Their values are written using software level memory, whereas the metadata refers to hardware level memory (see [High Level Operation](#high-level-operation)).

### RAM Parsing

During BOOTING, the binary image is parsed into RAM. The .text and .data values are copied to specific RAM regions while metadata guides the process. The illustrative code below demonstrates the final result (using hardware level memory addresses).

```
|------------| 0x0000
|.text values|
|------------| <.text metadata>
|...         |
|------------| 0x8000
|.data values|
|------------| <.data metadata>
|...         |
|------------| 0xC000
```

Overall, 64 KB and 32 KB are reserved explictly for .text and .data values, respectively. Regardless of these sizes, the binary image must fit within 64 KB itself.

## Mapped Registers
---
This section describes the mapped registers (and their related circuits). Each group of mapped registers (see [Block Diagram](#block-diagram)) is covered in its own sub-section.

*Note: This section uses a software level view of memory for addresses (see [High Level Operation](#high-level-operation)).*

### General Behavior

The following characteristics are true for all mapped registers (unless stated otherwise): 
- all mapped registers are read/written as 16-bit values
- upon a hardware reset, all mapped registers are reset to 0x0000
- unused mapped registers/bits are read only and equal to 0
- mapped registers' access permissions are constant for RUNNING and PAUSED
- mapped registers' functions are available for RUNNING and PAUSED

### Core Control Registers (CCTL)

The CCTRL registers provide various services related to the CORE. The tables below summarize their usage.

**_CCTRL_CTRL (0x8000)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:2     |r     |reserved for future use (value = 0)           |
|ovfEn     |1        |r/w   |stack overflow detection enable (0 = off)     |
|doPause   |0        |r/w   |uP pause signal (0 = unpause)                 |

**_CCTRL_SETP (0x8002)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|setpoint  |15:0     |r/w   |lowest valid stack pointer (R7) value         |

**_CCTRL_PC (0x8004)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|PC        |15:0     |r     |current PC value- used to fetch instructions  |

**_CCTRL_SP (0x8006)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|SP        |15:0     |r     |current stack pointer (R7)- used for detection|

CCTRL_CTRL's `doPause` bit is used to transition from RUNNING to PAUSED. It is automatically set by the CORE upon executing a halt/pause instruction (see ISA document PR2000), but can also be set/cleared manually.

CCTRL_CTRL's `ovfEn` bit enables stack overflow detection. Treating register 7 (referred to as R7) in the CORE's register file as a stack pointer, it asserts the stack overflow interrupt signal anytime R7 is less than CCTRL_SETP (and enabled).

For convenience, CCTRL_PC and CCTRL_SP are provided as read only registers. This is primarily for an external host (via JTAG) to be able to debug the software running on the uP.

### Nested Vector Interrupt Controller Registers (NVIC)

The NVIC registers handle the recording and passing of interrupts to the CORE. The tables below summarize their usage.

**_NVIC_ENABLE (0x8008)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:12    |r     |reserved for future use (value = 0)           |
|ovfEn     |11       |r/w   |CCTRL stack overflow enable (0 = off)         |
|exhEn     |10       |r/w   |GPIO high priority enable (0 = off)           |
|urxEn     |9        |r/w   |UART RX complete enable (0 = off)             |
|tm0En     |8        |r/w   |TMR0 overflow enable (0 = off)                |
|tm1En     |7        |r/w   |TMR1 overflow enable (0 = off)                |
|tm2En     |6        |r/w   |TMR2 overflow enable (0 = off)                |
|tm3En     |5        |r/w   |TMR3 overflow enable (0 = off)                |
|utxEn     |4        |r/w   |UART TX complete enable (0 = off)             |
|i2cEn     |3        |r/w   |I2C byte complete enable (0 = off)            |
|wdtEn     |2        |r/w   |WDT bark enable (0 = off)                     |
|exlEn     |1        |r/w   |GPIO low priority enable (0 = off)            |
|reserved  |0        |r     |reserved for future use (value = 0)           |

**_NVIC_FLAG (0x800A)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:12    |r     |reserved for future use (value = 0)           |
|ovfFlag   |11       |r/w   |CCTRL stack overflow pending (0 = no)         |
|exhFlag   |10       |r/w   |GPIO high priority pending (0 = no)           |
|urxFlag   |9        |r/w   |UART RX complete pending (0 = no)             |
|tm0Flag   |8        |r/w   |TMR0 overflow pending (0 = no)                |
|tm1Flag   |7        |r/w   |TMR1 overflow pending (0 = no)                |
|tm2Flag   |6        |r/w   |TMR2 overflow pending (0 = no)                |
|tm3Flag   |5        |r/w   |TMR3 overflow pending (0 = no)                |
|utxFlag   |4        |r/w   |UART TX complete pending (0 = no)             |
|i2cFlag   |3        |r/w   |I2C byte complete pending (0 = no)            |
|wdtFlag   |2        |r/w   |WDT bark pending (0 = no)                     |
|exlFlag   |1        |r/w   |GPIO low priority pending (0 = no)            |
|reserved  |0        |r     |reserved for future use (value = 0)           |

Each interrupt has a `En` and `Flag` bit (in NVIC_ENABLE and NVIC_FLAG, respectively). When an interrupt signal is asserted, the corresponding `Flag` bit is set. If an interrupt's `En` and `Flag` bit are set, the interrupt is valid and passed to the CORE.

When the CORE receives an interrupt, the PC and condition codes are saved, the PC is set based on the passed interrupt, and the CORE enters "interrupt mode". The equation below summarizes how the PC is set based on the passed interrupt.

`PC = <interrupt's 'En' bit field value> * 8`

In "interrupt mode", the CORE will not accept any new passed interrupts until a return from interrupt instruction is executed (see ISA document PR2000). Once executed, the CORE restores the PC and condition codes and re-enters "application mode".

For standard operation, the following recommendations are made:
- The first 96 bytes of the binary image should be reserved for interrupts
- Interrupt service routines should save/restore the register file as needed
- Interrupt service routines should clear their associated `Flag` bits

*Note: If multiple interrupts are valid, the valid interrupt with the highest `En` bit field value is passed to the CORE.*

*Note: Upon a hardware reset, the CORE starts in "application mode".*

### Watchdog Timer Registers (WDT)

The WDT registers handle the uP's watchdog timer, capable of causing a hardware reset or interrupt. The tables below summarize their usage.

**_WDT_CTRL (0x8010)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|isPaused  |15       |r     |indicator WDT is paused (0 = running)         |
|reserved  |14:4     |r     |reserved for future use (value = 0)           |
|intEn     |3        |r/w   |watchdog bark interrupt enable (0 = do reset) |
|reserved  |2        |r     |reserved for future use (value = 0)           |
|doReset   |1        |r/w   |manual hardware reset trigger (0 = inactive)  |
|doCancel  |0        |r/w   |pauses and resets WDT count (0 = inactive)    |

**_WDT_CNT (0x801A)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|cnt       |15:0     |r     |watchdog timer's current count                |

When the uP is RUNNING, WDT_CNT is automatically incremented at 1/16th CLK speed. Upon overflowing (ie after about 2\^20 CLK cycles), the WDT causes a hardware reset. Unlike most mapped registers, the WDT is active by default.

Any write to WDT_CTRL will reset WDT_CNT, and WDT_CTRL's `doCancel` bit, when set, will both reset and pause WDT_CNT (as observed by the `isPaused` bit). By contrast, setting WDT_CTRL's `doReset` bit will force a hardware reset.

WDT_CTRL's `intEn` bit allows a WDT_CNT overflow to generate a watchdog bark interrupt signal instead of a hardware reset. Setting this bit does not change how the `doReset` bit works.

*Note: WDT_CNT is paused when the uP is BOOTING or PAUSED.*

### General Purpose Input Output Registers (GPIO)

The GPIO registers handle the uP's digital IO pins. The tables below summarize their usage.

**_GPIO_CFG (0x8018)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15       |r     |reserved for future use (value = 0)           |
|uartAlt   |14       |r/w   |UART pins 14/15 control enable (0 = off)      |
|reserved  |13       |r     |reserved for future use (value = 0)           |
|i2cAlt    |12       |r/w   |I2C pins 12/13 control enable (0 = off)       |
|tmr2Alt   |11       |r/w   |TMR2 pin 11 control enable (0 = off)          |
|tmr3Alt   |10       |r/w   |TMR3 pin 10 control enable (0 = off)          |
|exhTrig   |9        |r/w   |high priority trigger config (0 = falling)    |
|exlTrig   |8        |r/w   |low priority trigger config (0 = rising)      |
|reserved  |7:0      |r     |reserved for future use (value = 0)           |

**_GPIO_INP (0x801A)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|input     |15:0     |r     |read levels from each pin                     |

**_GPIO_DIR (0x801C)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|direction |15:0     |r/w   |direction of each pin (0 = input)             |

**_GPIO_OUT (0x801E)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|output    |15:0     |r/w   |write levels for each pin                     |

Registers GPIO_DIR and GPIO_OUT control each pin's direction and output value, respectively (where bit field value corresponds to the effected pin). Similarly, GPIO_INP is used to read each pin, regardless of its `direction` bit.

GPIO_CFG allows for specific pins to be configured to be controlled by different mapped registers (via the `Alt` bits) instead of by the GPIO registers. Each bit's bit field value roughly determines the overridden pins.

Additionally, GPIO_CFG includes bits `exhTrig` and `exlTrig`. These control how the GPIO's high and low priority interrupt signals (tied to pins 9 and 8) are triggered, respectively. Both interrupt signals are always enabled.

*Note: Some `Alt` bits cause multiple pins to be overridden by different mapped registers.*

*Note: Both interrupt signals are enabled within GPIO, but not necessarily enabled in the NVIC registers.*

### Timer 0 and Timer 1 Registers (TMR0, TMR1)

The TMR0 and TMR1 registers handle the uP's two configurable count up timers. The tables below summarize their usage.

**_TMRx_CTRL (TMR0 = 0x8020, TMR1 = 0x8028)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r     |reserved for future use (value = 0)           |
|prescale  |7:4      |r/w   |prescalar value (0000 = 1:1, 1111 = 16:1)     |
|reserved  |3:1      |r     |reserved for future use (value = 0)           |
|enable    |0        |r/w   |timer count up enable (0 = off)               |

**_TMRx_CNT (TMR0 = 0x8022, TMR1 = 0x802A)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|cnt       |15:0     |r/w   |timer's current count                         |

**_TMRx_MAX (TMR0 = 0x8024, TMR1 = 0x802C)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|maxCnt    |15:0     |r/w   |max value TMRx_CNT is permitted               |

When the uP is RUNNING, the `enable` bit in TMRx_CTRL controls the auto incrementing of TMRx_CNT. The `prescale` bits (in the same register) can be used to configure the speed relative to the CLK, ranging from 1:1, 2:1, ..., up to 16:1.

TMRx_CNT's upper value is defined by TMRx_MAX. Upon the next increment, TMRx_CNT is reset to zero and a timer interrupt signal (specific to the individual timer) is generated. The interrupt signal is enabled along with the timer.

*Note: TMRx_CNT is paused when the uP is BOOTING or PAUSED.*

*Note: For prescale, 16:1 is read as "16 CLK ticks to 1 TMRx_CNT tick".*

### Timer 2 and Timer 3 Registers (TMR2, TMR3)

The TMR2 and TMR3 registers handle the uP's two count timers with PWM capabilities. The tables below summarize their usage.

**_TMRx_CTRL (TMR2 = 0x8030, TMR3 = 0x8038)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r     |reserved for future use (value = 0)           |
|prescale  |7:4      |r/w   |prescalar value (0000 = 1:1, 1111 = 16:1)     |
|reserved  |3:1      |r     |reserved for future use (value = 0)           |
|enable    |0        |r/w   |timer count up enable (0 = off)               |

**_TMRx_CNT (TMR2 = 0x8032, TMR3 = 0x803A)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|cnt       |15:0     |r/w   |timer's current count                         |

**_TMRx_MAX (TMR2 = 0x8034, TMR3 = 0x803C)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|maxCnt    |15:0     |r/w   |max value TMRx_CNT is permitted               |

**_TMRx_CMP (TMR2 = 0x8036, TMR3 = 0x803E)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|cmpr      |15:0     |r/w   |value compared with TMRx_CNT                  |

The TMR2 and TMR3 registers are supersets of the TMR0 and TMR1 registers. See [Timer 0 and Timer 1 Registers (TMR0, TMR1)](#timer-0-and-timer-1-registers-tmr0-tmr1) for relevant details.

The TMRx_CMP register is compared against TMRx_CNT to create an output signal. The equation below summarizes how the output signal is generated.

`output = (TMRx_CNT < TMRx_CMP)`

To export this output signal, the appropriate pin must be configured as an output (ie pin 11 for TMR2, pin 10 for TMR3) in GPIO_DIR, and the appropriate config bit (`tmr2Alt` for TMR2, `tmr3Alt` for TMR3) in GPIO_CFG must be set.

### Universal Asynchronous Receiver Transmitter Registers (UART)

The UART registers handle the uP's UART transceiver. The tables below summarize their usage.

**_UART_CTRL (0x8040)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|txIdle    |15       |r     |indicator TX channel is idle (0 = running)    |
|rxRdy     |14       |r     |indicator RX channel has new byte (0 = stale) |
|reserved  |13:1     |r/w   |reserved for future use (value = 0)           |
|enable    |0        |r/w   |UART channels enable (0 = off)                |

**_UART_BAUD (0x8042)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|baudRate  |15:1     |r/w   |baud rate of UART channels (in CLK ticks)     |
|reserved  |0        |r/w   |reserved for future use (value = 0)           |

**_UART_TX (0x8044)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r/w   |reserved for future use (value = 0)           |
|txByte    |7:0      |r/w   |byte to send over TX channel                  |

**_UART_RX (0x8046)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r/w   |reserved for future use (value = 0)           |
|rxByte    |7:0      |r     |byte received over RX channel                 |

When the uP is RUNNING, the `enable` bit in UART_CTRL enables the UART's channels/functions. Both channels use UART_BAUD to set their speed. Given a CLK and desired baud rate, the UART_BAUD value can be approximated using the equation below.

`value = floor((CLK / baud rate) - 1)`

The UART_TX register is used to send a byte over the TX channel. To send a byte, UART_TX must be written with the desired byte while the `txIdle` bit in UART_CTRL is set.

The UART_RX register is used to receive a byte over the RX channel. When a new byte is received, UART_RX is updated and the `rxRdy` bit in UART_CTRL is set. When UART_RX is read, the `rxRdy` bit is cleared. Newer received bytes overwrite older received bytes.

Each channel has an associated interrupt signal. When a byte is sent or a full byte is received, the respective interrupt signal is generated. Both interrupt signals are enabled with the UART.

To import/export the channels, pins 14 and 15 in GPIO_DIR must be configured as an input and output, respectively. Likewise, pin 14 in GPIO_CFG must be set.

*Note: The UART_BAUD value equation is an estimate- where accuracy tends to drop as CLK speed and desired baud rate get closer.*

*Note: All UART functions are disabled in BOOTING or PAUSED.*

*Note: When the uP transitions to PAUSED, both channels will finish their current transfers before entering PAUSED.*

### Inter Integrated Circuit Registers (I2C)

The I2C registers handle the uP's I2C controller. The tables below summarize their usage.

**_I2C_CTRL (0x8048)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|isIdle    |15       |r     |indicator I2C is idle (0 = running)           |
|lastAck   |14       |r     |value of previous ACK bit                     |
|reserved  |13:4     |r/w   |reserved for future use (value = 0)           |
|addStart  |3        |r/w   |add start condition to next transfer          |
|isWr      |2        |r/w   |write byte on next transfer (0 = read)        |
|addStop   |1        |r/w   |add stop condition to next transfer           |
|enable    |0        |r/w   |I2C controller enable (0 = off)               |

**_I2C_BAUD (0x804A)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|baudRate  |15:1     |r/w   |baud rate of I2C (in CLK ticks)               |
|reserved  |0        |r/w   |reserved for future use (value = 0)           |

**_I2C_DATA (0x804C)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r/w   |reserved for future use (value = 0)           |
|byte      |7:0      |r/w   |byte sent/received during transfer            |

When the uP is RUNNING, the `enable` bit in I2C_CTRL enables the I2C channel/functions. I2C_BAUD is used to set the I2C CLK speed. Given a  (system) CLK and desired baud rate, the I2C_BAUD value can be approximated using the equation below.

` value = floor((CLK / (baud rate * 2)) - 1)`

A byte transfer is started when I2C_DATA is written while I2C_CTRL's `isIdle` bit is set. Bits `addStart` and `addStop` add start/stop conditions to the transfer, while `isWr` determines the direction of the transfer.

For writing, the byte written to I2C_DATA while triggering the transfer is sent (while I2C_CTRL's `lastAck` bit records the received ACK bit). For reading, I2C_DATA is updated with the read byte at the end of the transfer.

Upon a completed transfer, an interrupt signal will be generated. The interrupt signal is enabled with the I2C.

To import/export the channel, pins 12 and 13 in GPIO_OUT must be set to 0. Likewise, pin 12 in GPIO_CFG must be set. Akin to a standard I2C network, this allows the I2C to actively pull down the pins, but relies on external pull-up resistors to drive the pins high.

*Note: The I2C_BAUD value equation is an estimate- where accuracy tends to drop as CLK speed and desired baud rate get closer.*

*Note: All I2C functions are disabled in BOOTING or PAUSED.*

*Note: When the uP transitions to PAUSED, the channel will finish its current transfer before entering PAUSED.*

*Note: Software is expected to use the `addStart` and `addStop` bits appropriately to open/close I2C connections.*

### Serial Peripheral Interface Registers (SPI)

The SPI registers handle the uP's SPI controller, which is hardwired to the storage chip/NVM. The tables below summarize their usage.

**_SPI_CTRL (0x8050)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|isIdle    |15       |r     |indicator SPI is idle (0 = running)           |
|isFree    |14       |r     |indicator SPI is unlocked (0 = locked)        |
|reserved  |13:0     |r     |reserved for future use (value = 0)           |

**_SPI_LOCK (0x8052)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|key       |15:0     |r/w   |value used to unlock SPI (ie 0x2024)          |

**_SPI_DATA (0x8054)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r     |reserved for future use (value = 0)           |
|byte      |7:0      |r/w   |data send/received during transfer            |

SPI_LOCK is used to allow writes to SPI_DATA and open a connection to the NVM. This is done by writing a specific key value to SPI_LOCK (see equation below). SPI_CTRL's `isFree` bit indicates if the written key value is correct.

`value = 0x2024`

While the uP is RUNNING and the `isFree` bit is set, a byte transfer can be started by writing SPI_DATA when SPI_CTRL's `isIdle` bit is set. The written byte is then sent (at CLK speed) to the NVM. Afterwards, the received byte is placed in SPI_DATA.

*Note: All SPI functions are disabled in BOOTING or PAUSED. Moreover, the SPI registers lose NVM access when not in RUNNING.*

*Note: When the uP transitions to PAUSED, the channel will finish its current transfer before entering PAUSED.*

*Note: Software is expected to use SPI_LOCK appropriately to open/close NVM connections.*

## Integration Considerations
---
This section provides details pertinent to integrating the RISCII softcore processor with the host FPGA and supporting external circuits. It covers 1) interface variation 2) characteristic matching and 3) pin pulling.

### Interface Variation

The uP is not designed to be directly run on the host FPGA. Instead, the uP should be be instantiated in a "Wrapper Module" that adapts the uP's pinout (see [Pinout](#pinout)) to the host FPGA's pinout.

Likewise, external circuits interfacing with the uP (eg memory chips) may have more connections than the uP's pinout. It is recommended that any extra pins either 1) be tied to VDD or GND or 2) be handled as an additional host FPGA pinout of the previously mentioned "Wrapper Module".

*Note: Additional pinout and "Wrapper Module" should be handled case by case- based on the host FPGA.*

### Characteristic Matching

External circuits that interface with the uP/FPGA must match its electrical and serial characteristics. This primarily applies to the memory chips (ie RAM and NVM).

**_Runtime Chip (RAM) Characteristics_**

The uP expects the runtime chip to have the following interface:
- 16-bit addressing
- 16-bit data (ie 2 bytes per address)
- 96 KB of storage (ie addresses 0x0000-0xBFFF are unique/valid)

Furthermore, the uP performs RAM accesses within the LOW level of the CLK cycle. In other words, a RAM access- plus any travel/computation time -must be one within half a CLK cycle.

**_Storage Chip (NVM) Characteristics_**

The uP expects the storage chip to implement a SPI interface, where CPOL/CPHA = 0 and rated for CLK speed (ie runs at same speed as uP).

Futhermore, the uP expects the storage chip has 64 KB of storage and supports streaming up to all bytes in ascending order. The illustrative code below demonstrates the expected send/recieve sequence.

```
NVM MOSI: [8-bit command code][0x0000]------------------------------
NVM MISO: ----------------------------[<= 64 KB ascending bitstream]
```

*Note: The "8-bit command code" is hardwired into the uP's BOOT circuit. Minor edits to BOOT may be required to interface with the storage chip.*

**_Additional Circuits_**

Any additional circuits interfaced with the uP/FPGA should consider the following characteristics during integration/design:
- Voltages (both analog and digital use cases)
- Current
- Timing (especially with respect to clock edges and computation delay)

### Pin Pulling

Some pins are expected to have explicit pull up/down resistors (for safetly and/or reliability). The table below summarizes the expected resistors.

|Pin Group Name|Expected Pull|Reason                                       |
|--------------|-------------|---------------------------------------------|
|RAM Enable    |LOW          |prevent errant RAM accesses                  |
|NVM CLK       |LOW          |prevent errant NVM accesses                  |
|NVM CSn       |HIGH         |prevent errant NVM accesses                  |
|GPIO 12/13    |HIGH         |required for I2C network functionality       |
|GPIO 14/15    |HIGH         |stabilize UART connection                    |
|JTAG TCK      |LOW          |prevent errant ticks upon hardware connection|
|JTAG TMS      |HIGH         |prevent errant ticks upon hardware connection|
|SYS Reset     |HIGH         |prevent errant external hardware reset       |
