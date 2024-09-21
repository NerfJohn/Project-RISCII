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
|BOOT      | Circuit to copy stored binary image to runtime memory on reset|
|CORE      | Core processor responsible for code execution                 |
|CCTRL     | Mapped CORE registers and stack overflow detection controls   |
|NVIC      | Interrupt Controller for enabling preemptive behavior         |
|WDT       | Watchdog timer for resetting hardware in event of SW failure  |
|GPIO      | General Purpose digital IO pins with some alternate functions |
|TMR0-1    | Two 16-bit configurable timers with overflow interrupts       |
|TMR2-3    | Two 16-bit configurable timers capable of PWM generation      |
|UART      | UART transceiver with configurable baud rate                  |

These blocks do not cover the entire uP design (ie smaller circuits exist to
route/synchronize signals), but reflect the primary "actors" and functions of the uP design at the high level.

## uP State Machine
---
Between various signals, the uP effectively implements a global "state machine" that dictates larger functionality (eg if firmware is running, which circuits control memory, etc). The high level states are as follows:

|State Name|SM Is Booted|SM Is Paused|Summary                              |
|----------|------------|------------|-------------------------------------|
|RUNNING   |HIGH        |LOW         |uP is running the binary firmware    |
|PAUSED    |HIGH        |HIGH        |uP is "paused", JTAG now in charge   |
|BOOTING   |LOW         |LOW         |uP is preparing to run binary image  |

Pins "SM Is Booted" and "SM Is Paused" act as external indicators of the uP's internal state (see "uP Pinout" below for more details about the pins).

In the RUNNING state, the CORE block executes the loaded firmware, exercising control of the uP's resources. In this mode, code (stored in the runtime chip) is executed, controlling the rest of the uP based on the program/functions being run.

In the PAUSED state, uP's resources temporarily cease operation (eg firmware execution pauses, timers freeze, etc). In this mode, the JTAG is given general control over the uP's resources. Note that while the PAUSED state is not reached until all resources are paused, some resources may take longer to pause than others (ie frequent pausing may cause performance variation).

In the BOOTING state, the BOOT block is given control over both memory chips (other resources are put on hold). In this mode, the binary image is copied from the storage chip into the runtime chip (see section "Binary Image Format" below). This state cannot be interrupted, though eventually terminates and does not prevent uP resources from being paused (ie prevents formal recognition of uP being paused, but not pause process).

## Mapped Registers
---
Some of the uP's internal resources contain registers that can be accessed using specific addresses in the data address space (ie SW addresses 0x8000-0xFFFF, HW adddresses 0xC000-0xFFFF). The following subsections cover each register in detail, though a summary of each register is as follows:

|Register Name |SW Address|Access|Description                              |
|--------------|----------|------|-----------------------------------------|
|CCTRL_CTRL    |0x8000    |r/w   |Controls for core processing features    |
|CCTRL_SETPOINT|0x8002    |r/w   |Setpoint for stack overflow detection    |
|CCTRL_PC      |0x8004    |r     |Current value of the PC                  |
|CCTRL_SP      |0x8006    |r     |Current value of the stack pointer (SP)  |
|NVIC_ENABLE   |0x8008    |r/w   |Interrupt enables (per interrupt)        |
|NVIC_FLAG     |0x800A    |r/w   |Status of interrupt signals              |
|WDT_CTRL      |0x8010    |r/w   |Controls/Status of Watchdog timer        |
|WDT_CNT       |0x8012    |r     |Current count of the WDT main counter    |
|GPIO_CFG      |0x8018    |r/w   |Gpio pin alternate functions/configs     |
|GPIO_INP      |0x801A    |r     |Read value for each GPIO pin             |
|GPIO_DIR      |0x801C    |r/w   |Controls read/write direction of pin     |
|GPIO_OUT      |0x801E    |r/w   |Write value for each GPIO pin            |
|TMR0_CTRL     |0x8020    |r/w   |Controls of timer 0                      |
|TMR0_CNT      |0x8022    |r/w   |Current count of timer 0                 |
|TMR0_MAX      |0x8024    |r/w   |Highest value timer 0 count can be       |
|TMR1_CTRL     |0x8028    |r/w   |Controls of timer 1                      |
|TMR1_CNT      |0x802A    |r/w   |Current count of timer 1                 |
|TMR1_MAX      |0x802C    |r/w   |Highest value timer 1 count can be       |
|TMR2_CTRL     |0x8030    |r/w   |Controls of timer 2                      |
|TMR2_CNT      |0x8032    |r/w   |Current count of timer 2                 |
|TMR2_MAX      |0x8034    |r/w   |Highest value timer 2 count can be       |
|TMR2_CMP      |0x8036    |r/w   |PWM comparator- "CNT \< CMP" comparison  |
|TMR3_CTRL     |0x8038    |r/w   |Controls of timer 3                      |
|TMR3_CNT      |0x803A    |r/w   |Current count of timer 3                 |
|TMR3_MAX      |0x803C    |r/w   |Highest value timer 3 count can be       |
|TMR3_CMP      |0x803E    |r/w   |PWM comparator- "CNT \< CMP" comparison  |
|UART_CTRL     |0x8040    |r/w   |UART control/status register             |
|UART_BAUD     |0x8042    |r/w   |UART baud rate (in system clock ticks)   |
|UART_TX       |0x8044    |r/w   |Byte to send over UART TX channel        |
|UART_RX       |0x8046    |r     |Byte received over UART RX channel       |

Unless otherwise stated, all registers are reset to a value of 0x0000 upon hardware reset. Likewise, any unspecified addresses are reserved registers with a read value of 0x0000.

### Core Controls (CCTRL) Registers

The CCTRL registers provide various controls and readings of the uP's core. Controls include interfaces for pausing and detecting stack overflow, whereas readings include copies of the current program counter (PC) and stack pointer (SP- equal to R7 in the core's register file).

The pause bit (active for value of 1) is primarily used by the core to continue pausing once the core itself has been paused, but can be used by software as well. When set, the JTAG or a power reset is required to clear the bit.

Enabling the stack overflow detector (active for value of 1) compares the SP to a given setpoint. The OVF interrupt is asserted anytime SP is less than the setpoint (compared as uint16 values). Due to this, the OVF interrupt may be active for contiguous cycles.

The CCTRL registers remain fully operational while the uP is in the PAUSED state.

**_CCTRL_CTRL (SW Address = 0x8000, HW Address = 0xC000)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:2     |r     |reserved for future use- default value(s) = 0 |
|ovdEn     |1        |r/w   |stack overflow detection enable               |
|doPause   |0        |r/w   |pauses uP (auto set by HLT instruction)       |

**_CCTRL_SETP (SW Address = 0x8002, HW Address = 0xC001)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|setpoint  |15:0     |r/w   |lowest valid stack pointer (ie "\<" trigger)  |

**_CCTRL_PC (SW Address = 0x8004, HW Address = 0xC002)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|PC        |15:0     |r     |current PC value- used to fetch instructions  |

**_CCTRL_SP (SW Address = 0x8006, HW Address = 0xC003)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|SP        |15:0     |r     |current stack pointer- checked by detector    |

### Nested Vector Interrupt Controller (NVIC) Registers

The NVIC registers control which interrupt's have been received an can be forwarded to the core for processing. Controls include individual enable and flag signals for each of the 11 interrupts. Logic shared between the NVIC and core implement priority and behavioral rules for interrupts.

Interrupts cause the core to save the current PC/CC values and enter an "interrupt mode". In this mode, the PC is initialized to a value equal to the serviced interrupt's bit field (ie in the NVIC registers) times 8. The core continues running in interrupt mode, ignoring other interrupts, until a JPR instruction with its "r" flag (see ISA) is executed. Once executed, the uP returns to the previous context (ie PC/CC values) preempted by the interrupt.

The enable bits determine if an interrupt's flag can be forwarded to the core (a value of 1 allowing passage). The flag bits record if an interrupt signal was detected (a value of 1 equaling detection). These bits are typically updated by hardware, though can be overwritten by the software directly.

If two or more interrupts are enabled and have been detected, the NVIC will forward the interrupt with the higher bit field (ie in the NVIC registers). Thus, while interrupts cannot pre-emptive other interrupts due to the core's "interrupt mode", they are executed in a prioritized manner.

In general, interrupts are expected to be enabled both in the NVIC and at their origin, arriving as pulses captured by the flag bits. It is expected that the flags are handled with care (as both hardware and software write to them- write conflicts are a potential issue).

The NVIC registers remain fully operational while the uP is in the PAUSED state. However, the core prevents interrupts from being processes until in the RUNNING state.

**_NVIC_ENABLE (SW Address = 0x8008, HW Address = 0xC004)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:12    |r     |reserved for future use- default value(s) = 0 |
|enable OVF|11       |r/w   |stack overflow detection enable (see CCTRL)   |
|enable EXH|10       |r/w   |external high priority pin enable (see GPIO)  |
|enable URX|9        |r/w   |UART RX byte complete enable (see UART)       |
|enable TM0|8        |r/w   |timer 0 overflow enable (see TMR0)            |
|enable TM1|7        |r/w   |timer 1 overflow enable (see TMR1)            |
|enable TM2|6        |r/w   |timer 2 overflow enable (see TMR2)            |
|enable TM3|5        |r/w   |timer 3 overflow enable (see TMR3)            |
|enable UTX|4        |r/w   |UART TX byte complete enable (see UART)       |
|reserved  |3        |r/w   |reserved for future use- default value(s) = 0 |
|reserved  |2        |r/w   |reserved for future use- default value(s) = 0 |
|enable EXL|1        |r/w   |external low priority pin enable (see GPIO)   |
|reserved  |0        |r/w   |reserved for future use- default value(s) = 0 |

**_NVIC_FLAG (SW Address = 0x800A, HW Address = 0xC005)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:12    |r     |reserved for future use- default value(s) = 0 |
|OVF flag  |11       |r/w   |stack overflow detection status (see CCTRL)   |
|EXH flag  |10       |r/w   |external high priority pin status (see GPIO)  |
|URX flag  |9        |r/w   |UART RX byte complete flag (see UART)         |
|TM0 flag  |8        |r/w   |timer 0 overflow status (see TMR0)            |
|TM1 flag  |7        |r/w   |timer 1 overflow status (see TMR1)            |
|TM2 flag  |6        |r/w   |timer 2 overflow status (see TMR2)            |
|TM3 flag  |5        |r/w   |timer 3 overflow status (see TMR3)            |
|UTX flag  |4        |r/w   |UART TX byte complete flag (see UART)         |
|reserved  |3        |r/w   |reserved for future use- default value(s) = 0 |
|reserved  |2        |r/w   |reserved for future use- default value(s) = 0 |
|EXL flag  |1        |r/w   |external low priority pin status (see GPIO)   |
|reserved  |0        |r/w   |reserved for future use- default value(s) = 0 |

### Watchdog Timer (WDT) Registers

The WDT registers control the uP's watchdog timer- a special timer used to reset the automatically reset the hardware for a software error. The timer's "fuse" is implicitly paused when not in the RUNNING mode and can be reset or disarmed using the controls. A manual hardware reset can also be performed.

By default, the timer runs in the RUNNING state, with a 2\^20 cycle fuse before it forces a hardware reset. This fuse can be paused by leaving the RUNNING state, reset (to 2\^20 cycles) by any write to WDT_CTRL, and reset+disarmed by setting the "cancel" bit in WDT_CTRL. A read-only "paused" status bit under WDT_CTRL indicates if the fuse/timer is paused.

Additionally, a manual hardware reset can be performed by setting the "reset" bit under WDT_CTRL. This will force a hardware reset to occur regardless of the uP state and WDT timer's fuse.

The WDT registers are only fully operationl while the uP is in the RUNNING state. The WDT timer is effectively paused in the BOOTING and PAUSED uP states.

**_WDT_CTRL (SW Address = 0x8010, HW Address = 0xC008)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|isPaused  |15       |r     |indicator for WDT being paused (active high)  |
|reserved  |14:2     |r     |reserved for future use- default value(s) = 0 |
|reset     |1        |r/w   |manual trigger HW reset (active high)         |
|cancel    |0        |r/w   |stops the WDT from counting (active high)     |

**_WDT_CNT (SW Address = 0x801A, HW Address = 0xC009)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|count     |15:0     |r     |current count of WDT's main counter           |

### General Purpose Input/Output (GPIO) Registers

The GPIO registers control the uP's 16 general purpose digital pins. Pins can be read, configured as an input/output, and set to digital value. Some pins can also be configured to be used by other peripherals or generate externally sourced interrupts.

All pins are capable of generic input/output capabilities. Controls are split across 3 registers, with bit field values indicating which pin they correspond to. Writing a value to a pin only works when the pin's direction is outward (ie DIR value of 1). Pins can always be read, regardless of direction.

Two pins (bit field's 8 and 9) has built-in edge detecting interrupts EXL and EXH, respectively. Each interrupt can be configured to trigger on a specific edge and are effectively always enabled (ie will set their NVIC flag bits triggered- exercise caution when first enabling).

Some pins possess alternate functions, often related to other peripherals (eg PWM timers can export their signal to a GPIO). The CFG register is used to select if these pins are used as GPIO pins or used by the associated peripheral. In any case, a peripheral requires the CFG register (and likely DIR register) to be set properly to control the GPIO pin as expected.

The GPIO registers remain fully operational while the uP is in the PAUSED state.

**_GPIO_CFG (SW Address = 0x8018, HW Address = 0xC00C)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15       |r     |reserved for future use- default value(s) = 0 |
|Alt UART  |14       |r/w   |GPIO 14/15 = UART RX/TX (active high)         |
|reserved  |13       |r     |reserved for future use- default value(s) = 0 |
|TODO      |12       |r/w   |not implemented yet, but readable/writable    |
|Alt TMR2  |11       |r/w   |GPIO 11 = TMR2 PWM (active high)              |
|Alt TMR3  |10       |r/w   |GPIO 10 = TMR3 PWM (active high)              |
|EXH trig  |9        |r/w   |EXH edge trigger- falling = 0, rising = 1     |
|EXL trig  |8        |r/w   |EXL edge trigger- falling = 0, rising = 1     |
|reserved  |7:0      |r     |reserved for future use- default value(s) = 0 |

**_GPIO_INP (SW Address = 0x801A, HW Address = 0xC00D)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|read bits |15:0     |r     |read values, regardless of direction          |

**_GPIO_DIR (SW Address = 0x801C, HW Address = 0xC00E)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|direction |15:0     |r/w   |pin direction- read = 0, write = 1            |

**_GPIO_OUT (SW Address = 0x801E, HW Address = 0xC00F)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|write bits|15:0     |r/w   |write values, only used if pin is writing     |

### Timer 0 (TMR0) and Timer 1 (TMR1) Registers

The TMR0 and TMR1 registers control timers 0 and 1 respectively. Each timer is a basic count-up timer, featuring an enable bit, a configurable prescale, a configurable max count, and interrupt upon count overflow.

The CTRL register allows for enabling the timer, as well as setting a prescalar value- a ratio between when the timer's CNT increments and the system clock. This effectively creates a "counter for the counter", which is reset anytime the CTRL register is written to or the uP is the PAUSED state.

The MAX register allows for choosing the highest value the timer's CNT can reach before overflowing. The CNT register is reset anytime the MAX register is written to. Note that setting the CNT register value to be above the MAX register value results in undefined behavior.

Upon every overflow, an interrupt pulse is generated and sent to the NVIC (see NVIC). This pulse is enabled by enabling the timer (though not necessarily enabled in the NVIC). This overflow point adjusts with the MAX register.

The TMR0-1 registers are only fully operationl while the uP is in the RUNNING state. The timers are effectively paused in the BOOTING and PAUSED uP states.

**_TMR0_CTRL (SW Address = 0x8020, HW Address = 0xC010)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r     |reserved for future use- default value(s) = 0 |
|prescale  |7:4      |r/w   |prescalar- "0000" = 1:1 -\> "1111" = 16:1     |
|reserved  |3:1      |r     |reserved for future use- default value(s) = 0 |
|enable    |0        |r/w   |enables timer for incrementing (active high)  |

**_TMR0_CNT (SW Address = 0x8022, HW Address = 0xC011)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|count     |15:0     |r/w   |current count of the timer                    |

**_TMR0_MAX (SW Address = 0x8024, HW Address = 0xC012)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|maxCount  |15:0     |r/w   |max value CNT register can increment to       |

**_TMR1_CTRL (SW Address = 0x8028, HW Address = 0xC014)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r     |reserved for future use- default value(s) = 0 |
|prescale  |7:4      |r/w   |prescalar- "0000" = 1:1 -\> "1111" = 16:1     |
|reserved  |3:1      |r     |reserved for future use- default value(s) = 0 |
|enable    |0        |r/w   |enables timer for incrementing (active high)  |

**_TMR1_CNT (SW Address = 0x802A, HW Address = 0xC015)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|count     |15:0     |r/w   |current count of the timer                    |

**_TMR1_MAX (SW Address = 0x802C, HW Address = 0xC016)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|maxCount  |15:0     |r/w   |max value CNT register can increment to       |

### Timer 2 (TMR2) and Timer 3 (TMR3) registers

The TMR2 and TMR3 registers control timers 2 and 3 respectively. They posses the same capabilities/controls as timers 0 and 1 (see TMR0 and TMR1), but also include PWM capabilities, which can be exported via GPIOs.

Compared to TM0 and TMR1, timers 2 and 3 add the CMP register, which is used to generate a PWM signal based on CNT register (ie expression "CNT \< CMP" used). Timer 2's PWM signal can be exported through GPIO pin 11 while Timer 3's PWM signal can be exported through GPIO pin 10.

To export the PWM signals, the selected GPIO pin must be configured as an output and have its associated alternate function selected (see GPIO). Doing so will re-assign the GPIO pin as a PWM output pin.

The TMR2-3 registers are only fully operationl while the uP is in the RUNNING state. The timers are effectively paused in the BOOTING and PAUSED uP states.

**_TMR2_CTRL (SW Address = 0x8030, HW Address = 0xC018)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r     |reserved for future use- default value(s) = 0 |
|prescale  |7:4      |r/w   |prescalar- "0000" = 1:1 -\> "1111" = 16:1     |
|reserved  |3:1      |r     |reserved for future use- default value(s) = 0 |
|enable    |0        |r/w   |enables timer for incrementing (active high)  |

**_TMR2_CNT (SW Address = 0x8032, HW Address = 0xC019)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|count     |15:0     |r/w   |current count of the timer                    |

**_TMR2_MAX (SW Address = 0x8034, HW Address = 0xC01A)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|maxCount  |15:0     |r/w   |max value CNT register can increment to       |

**_TMR2_CMP (SW Address = 0x8036, HW Address = 0xC01B)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|comparator|15:0     |r/w   |compare value- does "CNT \< CMP" comparison   |

**_TMR3_CTRL (SW Address = 0x8038, HW Address = 0xC01C)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r     |reserved for future use- default value(s) = 0 |
|prescale  |7:4      |r/w   |prescalar- "0000" = 1:1 -\> "1111" = 16:1     |
|reserved  |3:1      |r     |reserved for future use- default value(s) = 0 |
|enable    |0        |r/w   |enables timer for incrementing (active high)  |

**_TMR3_CNT (SW Address = 0x803A, HW Address = 0xC01D)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|count     |15:0     |r/w   |current count of the timer                    |

**_TMR3_MAX (SW Address = 0x803C, HW Address = 0xC01E)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|maxCount  |15:0     |r/w   |max value CNT register can increment to       |

**_TMR3_CMP (SW Address = 0x803E, HW Address = 0xC01F)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|comparator|15:0     |r/w   |compare value- does "CNT \< CMP" comparison   |

### Universal Asynchronous Receiver/Transmitter (UART) Registers

The UART registers control the uP's UART transceiver. The UART features one transmit/TX and receive/RX channel each with a shared baud rate. Interrupts and status bits are available to indicate when a byte has been sent/received. The UART is exported through configured GPIO pins.

The UART_CTRL register is used to enable both channels (and interrupts), as well as report the current status of each channel. The status bits indicate when another byte can be sent and if a new, unread byte has been received.

The speed of both channels is configured using the UART_BAUD register. The baud rate must be given in terms of system clock ticks (and even). If the system clock frequency and desired baud rate are known, the correct value can be estimated using the following equation:

<code>floor_by_2((sys_clk_Hz \/ baud_rate) - 1) = UART_BAUD</code>

The UART_TX register is used to provide a byte to send. Writing to this register will use the lowest 8 bits as the input and begin transmitting the byte, but only if the following conditions are true:
- The TX channel is idle/not transmitting
- The UART is enabled
- The uP is in the RUNNING state (ie not paused or booting)

THe UART_RX register is used to read a received byte. Reading this register (while the uP is in the RUNNING state) will clear UART_CTRL's "RX" status bit, whereas the same bit is set everytime this register is updated. The register, however, is only updated with the most recently received byte if the following conditions are true:
- The RX channel has received a full byte
- The UART is enabled
- The uP is in the RUNNING state (ie not paused or booting)

Each channel has an associated interrupt for when a full byte is transmitted/received. Both are enabled with the UART and require the UART to be enabled the moment the signal is generated to be registered by the NVIC (see NVIC).

To use the UART, the GPIO must be configured for UART use. The directions must be set accordingly (ie pin 14 = input, pin 15 = output) and the alternate UART pin function selected (see GPIO). Doing so will re-assign pins 14 and 15 to the UART's control.

The UART registers are only fully operation while the uP is in the RUNNING state. Once a pause is started, the UART finishes any TX/RX processes (without generating interrupts/updating UART_RX), then pauses.

**_UART_CTRL (SW Address = 0x8040, HW Address = 0xC020)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|tx_idle   |15       |r     |indicator if TX channel is idle (active high) |
|rx_rdy    |14       |r     |indicator that RX channel has new, unread byte|
|reserved  |13:1     |r/w   |reserved for future use- default value(s) = 0 |
|enable    |0        |r/w   |enables UART channels (active high)           |

**_UART_BAUD (SW Address = 0x8042, HW Address = 0xC021)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|baud_rate |15:1     |r/w   |baud rate of UART (in system clock ticks)     |
|reserved  |0        |r/w   |reserved for future use- default value(s) = 0 |

**_UART_TX (SW Address = 0x8044, HW Address = 0xC022)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r/w   |reserved for future use- default value(s) = 0 |
|tx_byte   |7:0      |r/w   |byte to send over TX channel                  |

**_UART_RX (SW Address = 0x8046, HW Address = 0xC023)_**

|Field Name|Bit Field|Access|Description                                   |
|----------|---------|------|----------------------------------------------|
|reserved  |15:8     |r/w   |reserved for future use- default value(s) = 0 |
|rx_byte   |7:0      |r     |most recently received byte from RX channel   |

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

Furthermore, specific interface requirements (eg timing, command codes, etc) will need to be considered while integrating chips with the uP. At present, Most assumptions made by the uP (beyond inferred use of pinouts) is the SPI interface with the storage chip, which must meet the following criteria:
- SPI mode 0 (CPOL/CPHA = 0), MSb first
- 8-bit command codes, 16-bit addresses
- "read bitstream" command with sequence "command","address","read out"
- "read bitstream" command of "0x03" (must change design based on chip)

As stated, the exact command byte used to trigger a read is hardwired into the design (in file "processor/DevProject/src/ProcBlocks/BootImage.v") and must be changed based on the exact storage chip used.

Lastly, be aware of any timing considerations (hold times on falling edges, access times, etc). Specifically, note that both memory chip interfaces update pins on the falling edge of the system clock- meaning that memory accesses likely need to happen within half a clock cycle (ie round trip travel + chip and uP delays).

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
|GPIO 14/15    |up/HIGH      |help stabilize UART serial pins              |
