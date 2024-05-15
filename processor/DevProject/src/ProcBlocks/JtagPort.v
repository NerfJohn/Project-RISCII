/*
 * JtagPort.v
 *
 * "Circuit to parse JTAG signals into requests and control signals"
 */
module JtagPort (
    // Typical I/O of JTAG port.
    input         i_TCK,
    input         i_TDI,
    output        o_TDO,
    input         i_TMS,
    
     // MCU state for status report + control signal computation.
     input         i_isBooted,
     input         i_isPaused,
    
     // Memory control connections.
     output [15:0] o_memAddr,
     inout  [15:0] io_memData,
     output        o_memWr,
     output        o_memEn,
     
     // Storage control connections.
     output        o_storeEn,
     
     // B-Scan control connections.
     output        o_scanEn,
    output        o_scanShift,    
    
     // Run/Pause control signal.
     output        o_doPause,
    
    // Common signals.
    input         i_clk,
    input         i_rstn
);

/*
 * JTAG Port- "Maintenance Hatch" of the MCU
 *
 * Port to aid with debugging and programming. Host can shift commands and data
 * in/out of JTAG to control, read, and write various resources. MCU must be 
 * booted (and generally paused) to properly use JTAG.
 *
 * Pins used similar to a SPI connection (MSb, CPOL, CPHA = 0), expect TMS
 * signal is used to traverse an internal state machine, NOT simply enable the
 * port. TDI, TMS, and TDO should update on TCK's rising edge.
 *
 * JTAG can access three resources: Memory, Storage and the Boundary Scan
 * Register. Each access works slightly differently, but all can only be
 * accessed one at a time (enforced by JTAG port).
 *
 * == JTAG Pinout ==
 * pin name | default level | direction |description
 * ---------+---------------+-----------+-----------
 * TCK      | 0             | -> MCU    | Test Clock, for transfer rate
 * TMS      | 1             | -> MCU    | Test Select, for selecting state
 * TDI      | 0             | -> MCU    | Test Data In, for input data
 * TDO      | 0             | MCU ->    | Test Data Out, for output data
 *
 * == JTAG State Machine ==
 * state name | state value | next state for TMS = 0,1 | desc.
 * -----------+-------------+--------------------------+------
 * IDLE       | 3'b000      | 0 -> I-SEL , 1 -> IDLE   | idle/reset state
 * I-SEL      | 3'b001      | 0 -> I-SHFT, 1 -> D-SEL  | select "shift command"
 * D-SEL      | 3'b101      | 0 -> D-SHFT, 1 -> IDLE   | select "shift data"
 * I-SHFT     | 3'b100      | 0 -> I-SHFT, 1 -> UPDATE | shifting in command
 * D-SHFT     | 3'b010      | 0 -> D-SHFT, 1 -> IDLE   | shifting in data
 * UPDATE     | 3'b110      | 0 -> IDLE  , 1 -> IDLE   | execute given command
 *
 * == JTAG Accessible Registers ==
 * register    | width  | value on read  | desc.
 * ------------+--------+----------------+------
 * instruction | 8-bit  | status bits    | report status, execute given command
 * data        | 16-bit | context driven | data for later use, often masked over
 *
 * == Status Bits ==
 * field | name     | desc.
 * ------+----------+------
 * [7:2] | ---      | reserved- read as 0s
 * [1]   | isPaused | indicates if MCU is in "paused" state
 * [0]   | isBooted | indicates if MCU has booted (ie NOT in "booting" state)
 *
 * == Available Commands ==
 * command        | code | requires | desc.
 * ---------------+------+----------+------
 * No Operation   | 0x00 | ---      | (Has no effect on JTAG/MCU)
 * Set Address    | 0x01 | ---      | Set/save address as Memory address
 * Read Memory    | 0x02 | Paused   | Read Memory address to data register
 * Write Memory   | 0x03 | Paused   | Write data register to Memory address
 * Access B-Scan  | 0x04 | Paused   | B-Scan register mapped over data register
 * Access Storage | 0x05 | Paused   | Storage mapped over data register
 * Run MCU        | 0x06 | Booted   | Set MCU to 'running' state
 * Pause MCU      | 0x07 | Booted   | Set MCU to 'paused' state
 *
 * Design Notes:
 * 1) Only TCK edges within "SHFT" state shift (including exiting edge!)
 * 2) Design contains Mealy aspects (ie state + TMS driven signals)
 * 3) Even when masked, data register gets updated
 * 4) Every instruction shift is followed by an attempt to execute
 * 5) Memory enable with 'exeCmd', other enables happen just after
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to state machine.
wire [2:0] stateD, stateQ;
wire [2:0] nextState1XX, nextState10X;
wire [2:0] nextState0XX, nextState00X;

// Computed signals related to JTAG state + transitions.
wire inDShift, inUpdate;
wire cmdPulseD, cmdPulseQ;
wire getStatus, exeCmd;

// Wires related to JTAG instruction register.
wire [7:0] iRegD, iRegQ;
wire [1:0] iRegLowBits;

// Computed signals related to given instruction/status.
wire is3BitCmd, inControl;
wire isAddrCmd, isMemCmd;
wire isScanCmd, isStorageCmd;
wire isMcuCmd;

// Wires related to JTAG data register.
wire [15:0] dRegD, dRegQ;
wire        dRegEn;

// Wires related to determining returned shift bit.
wire retShiftBit;

// Wires related to memory connector elements.
wire[15:0] addrD, addrQ;
wire       addrEn;
wire[15:0] dataTriA, dataTriY;
wire       dataTriEn;

// Wires related to resource acquisition.
wire scanD,  scanQ,  scanEn;
wire storeD, storeQ, storeEn;
wire memEn;

// Wires related to pausing MCU.
wire pauseD, pauseQ;
wire pauseEn;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// State machine of external JTAG- clocked at TCK speed, NOT core speed.
DffAsynch STATE[2:0] (
    .D(stateD),
    .Q(stateQ),
    .clk(i_TCK),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Quick register used to generate 1-core cycle pulse for Memory accesses.
DffSynch CMD_PULSE (
    .D(cmdPulseD),
    .Q(cmdPulseQ),
    .clk(i_clk),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Instruction register- 8-bit command controlling JTAG Port actions.
DffAsynch I_REG[7:0] (
    .D(iRegD),
    .Q(iRegQ),
    .clk(i_TCK),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Data register- 16-bit value used to pass data between Host and JTAG.
DffAsynchEn D_REG[15:0] (
    .D(dRegD),
    .Q(dRegQ),
    .S(dRegEn),
    .clk(i_TCK),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Elements related to memory controls.
DffSynchEn ADDR[15:0] (
    .D(addrD),
    .Q(addrQ),
    .S(addrEn),
    .clk(i_clk),
    .rstn(i_rstn)
);
Tristate DATA_TRI[15:0] (
    .A(dataTriA),
    .Y(dataTriY),
    .S(dataTriEn)
);

//------------------------------------------------------------------------------
// Registers to control register acquisition.
DffSynchEn SCAN_LOCK (
    .D(scanD),
    .Q(scanQ),
    .S(scanEn),
    .clk(i_clk),
    .rstn(i_rstn)
);
DffSynchEn STORE_LOCK (
    .D(storeD),
    .Q(storeQ),
    .S(storeEn),
    .clk(i_clk),
    .rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Register used to control if MCU is paused.
DffSynchEn PAUSE_EN (
    .D(pauseD),
    .Q(pauseQ),
    .S(pauseEn),
    .clk(i_clk),
    .rstn(i_rstn)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Determine next state/update state.
Mux2 M0[2:0] (
    .A({2'b10, i_TMS}), 
    .B({2'b00, ~i_TMS}), 
    .S(stateQ[0]), 
    .Y(nextState00X)
);
Mux2 M1[2:0] (
    .A({1'b0, ~i_TMS, 1'b0}),
    .B(nextState00X),
    .S(stateQ[1]),
    .Y(nextState0XX)
);
Mux2 M2[2:0] (
    .A({1'b0, ~i_TMS, 1'b0}), 
    .B({1'b1, i_TMS, 1'b0}), 
    .S(stateQ[0]), 
    .Y(nextState10X)
);
assign nextState1XX = nextState10X & {3{~stateQ[1]}}; // "Mux w/ 0"
Mux2 M3[2:0] (
    .A(nextState1XX), 
    .B(nextState0XX), 
    .S(stateQ[2]),
    .Y(stateD)
);

//------------------------------------------------------------------------------
// Compute signals based on JTAG state.
assign inUpdate  = stateQ[2] & stateQ[1] & ~stateQ[0];
assign inDShift  = ~stateQ[2] & stateQ[1] & ~stateQ[0];
assign cmdPulseD = inUpdate;
assign getStatus = ~stateQ[2] & ~stateQ[1] & stateQ[0] & ~i_TMS;
assign exeCmd    = inUpdate & ~cmdPulseQ;

//------------------------------------------------------------------------------
// Determine instruction register input (shift vs status).
Mux2 M4[1:0] (
    .A({i_isPaused, i_isBooted}), // status
    .B({iRegQ[0], i_TDI}),        // shifted
    .S(getStatus),                // use status bits when requested
    .Y(iRegLowBits)
);
assign iRegD = {{iRegQ[6:1]} & {6{~getStatus}}, iRegLowBits};

//------------------------------------------------------------------------------
// Computer signals based on instruction/status.
assign is3BitCmd    = ~(|iRegQ[7:3]);
assign inControl    = i_isPaused & i_isBooted;
assign isAddrCmd    = is3BitCmd & ~iRegQ[2] & ~iRegQ[1] & iRegQ[0];
assign isMemCmd     = is3BitCmd & ~iRegQ[2] & iRegQ[1];
assign isScanCmd    = is3BitCmd & iRegQ[2] & ~iRegQ[1] & ~iRegQ[0];
assign isStorageCmd = is3BitCmd & iRegQ[2] & ~iRegQ[1] & iRegQ[0];
assign isMcuCmd     = is3BitCmd & iRegQ[2] & iRegQ[1];

//------------------------------------------------------------------------------
// Determine data register input/enable (shift vs memory... or nothing).
Mux2 M5[15:0] (
    .A({dRegQ[14:0], i_TDI}),        // shift  
    .B(io_memData),                  // memory
    .S(inDShift),                    // just ensure "shift" in D-SHFT
    .Y(dRegD)
);
assign dRegEn = inDShift | exeCmd;   // ...or nothing

//------------------------------------------------------------------------------
// Handle return of shifted out bit from JTAG registers.
Mux2 M6 (
    .A(dRegQ[15]),
    .B(iRegQ[7]),
    .S(inDShift),
    .Y(retShiftBit)
);

//------------------------------------------------------------------------------
// Handle memory connections (except for "enable"- handled elsewhere).
assign addrD     = dRegQ;
assign addrEn    = isAddrCmd & exeCmd;
assign dataTriA  = dRegQ;
assign dataTriEn = isMemCmd & iRegQ[0] & exeCmd & inControl;

//------------------------------------------------------------------------------
// Handle acquiring of hardware resources (attempted on every command).
assign scanD   = isScanCmd & inControl;
assign scanEn  = exeCmd;
assign storeD  = isStorageCmd & inControl;
assign storeEn = exeCmd;
assign memEn   = isMemCmd & exeCmd & inControl;

//------------------------------------------------------------------------------
// Handle pausing/unpausing of MCU from JTAG.
assign pauseD  = iRegQ[0];                        // run (0x06) vs pause (0x07)
assign pauseEn = isMcuCmd & exeCmd & i_isBooted; // still need to boot to run

//------------------------------------------------------------------------------
// Handle main JTAG connection output.
assign o_TDO = retShiftBit;

//------------------------------------------------------------------------------
// Output Memory control/data signals.
assign o_memAddr   = addrQ;
assign io_memData  = dataTriY;
assign o_memWr     = iRegQ[0];         // read (0x02) vs. write (0x03)
assign o_memEn     = memEn;

//------------------------------------------------------------------------------
// Output Storage control signals.
assign o_storeEn   = storeQ & inDShift;

//------------------------------------------------------------------------------
// Output Boundary Scan control signals.
assign o_scanEn    = scanQ & ~memEn;   // mem accesses while lock is updating
assign o_scanShift = scanQ & inDShift;

//------------------------------------------------------------------------------
// Output MCU Run/Pause control signal.
assign o_doPause = pauseQ;

endmodule
