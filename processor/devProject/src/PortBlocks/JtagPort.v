/*
 * JtagPort.v
 *
 * "Circuit to parse JTAG signals into requests and control signals"
 */
module JtagPort (
    // Typical I/O of JTAG port.
    input  jtagTCK,
    input  jtagTDI,
    output jtagTDO,
    input  jtagTMS,
    
    // Inputs for reporting status via "instruction" register.
    input  isBooted,
    input  isPaused,
    
    // SRAM chip access connections.
    output [15:0] sramAddr,
    inout  [15:0] sramData,
    output        sramWr,
    output        sramEn,
    
    // Control signals for relay/pausing.
    output enScanRelay,
    output enSPIRelay,
    output enPaused,
    
    // Common signals.
    input  clk,
    input  rstn
);

/* JTAG Port:
 *
 * "Maintenance Hatch" port for debugging and programming MCU. Gives host direct
 * MSB shift access to 8-bit instruction/command and 16-bit data registers.
 * Below describes JTAG's state machine, status bits, and available commands.
 *
 * state name | state value | next state for TMS = 0,1 | desc.
 * -----------+-------------+--------------------------+------
 * IDLE       | 3'b000      | 0 -> I-SEL , 1 -> IDLE   | idle/reset state
 * I-SEL      | 3'b001      | 0 -> I-SHFT, 1 -> D-SEL  | select "shift command"
 * D-SEL      | 3'b101      | 0 -> D-SHFT, 1 -> IDLE   | select "shift data"
 * I-SHFT     | 3'b100      | 0 -> I-SHFT, 1 -> UPDATE | shifting in command
 * D-SHFT     | 3'b010      | 0 -> D-SHFT, 1 -> IDLE   | shifting in data
 * UPDATE     | 3'b110      | 0 -> IDLE  , 1 -> IDLE   | execute given command
 *
 * bit field | field name | desc.
 * ----------+------------+------
 * [7:2]     | ---        | reserved- always read as 0
 * [1]       | isPaused   | indicates if MCU is currently is 'paused' state
 * [0]       | isBooted   | indicates if MCU is NOT currently in 'booting' state
 *
 * command       | code | requires | desc.
 * --------------+------+----------+------
 * No Operation  | 0x00 | ---      | (Has no effect on JTAG/MCU)
 * Set Address   | 0x01 | ---      | Set/save address as RAM address
 * Read RAM      | 0x02 | Paused   | Read saved RAM address to data register
 * Write RAM     | 0x03 | Paused   | Write data register to saved RAM address
 * Relay to Scan | 0x04 | Paused   | Boundary scan mapped over data register
 * Relay to SPI  | 0x05 | Paused   | EEPROM SPI mapped over data register
 * Run MCU       | 0x06 | Booted   | Set MCU to 'running' state
 * Pause MCU     | 0x07 | Booted   | Set MCU to 'paused' state
 *
 * Design Notes:
 * 1) Only TCK edges within "SHFT" state count (including exiting edge!).
 * 2) UPDATE signals occur at TCK speed (ie RAM accesses are held/repeated).
 * 3) Design contains Mealy aspects (ie state + TMS driven signals).
 * 4) Even when relaying, data registers gets updated.
 * 5) Every instruction shift is followed by an attempt to execute
 */

/////////////////////////
// -- Signals/Wires -- //
/////////////////////////

// State machine wires.
wire[2:0] stateD, stateQ;

// Nets involved in computing next state.
wire[2:0] nextState1XX, nextState10X;
wire[2:0] nextState0XX, nextState00X;

// Computed signals regarding current/next state.
wire inDShift, inUpdate;
wire runCmd;

// Instruction register wires/computed input.
wire[7:0] iRegD, iRegQ;
wire[1:0] nextIRegBits;

// Data register wires.
wire[15:0] dRegD, dRegQ;
wire       dRegEn;

// Computed signals regarding command/data input.
wire is3Bit, inControl;

// RAM access register/device wires.
wire[15:0] addrD, addrQ;
wire       addrEn;
wire[15:0] dTriA, dTriY;
wire       dTriEn;

// Relay control wires.
wire scanD, scanQ, scanEn;
wire spiD, spiQ, spiEn;

// Pause control wires.
wire pauseD, pauseQ, pauseEn;

////////////////////////////
// -- Blocks/Instances -- //
////////////////////////////

// State machine of external JTAG- clocked at TCK speed, NOT core speed.
DffAsynch STATE[2:0] (
    .D(stateD),
    .Q(stateQ),
    .clk(jtagTCK),
    .rstn(rstn)
);

// Instruction register- 8-bit command controlling JTAG (and MCU).
DffAsynch I_REG[7:0] (
    .D(iRegD),
    .Q(iRegQ),
    .clk(jtagTCK),
    .rstn(rstn)
);

// Data register- 16-bit value acting as RAM address or RAM data.
DffAsynchEn D_REG[15:0] (
    .D(dRegD),
    .Q(dRegQ),
    .S(dRegEn),
    .clk(jtagTCK),
    .rstn(rstn)
);

// Registers/devices used for RAM accessing.
DffEn ADDR[15:0] (
    .D(addrD),
    .Q(addrQ),
    .S(addrEn),
    .clk(clk),
    .rstn(rstn)
);
Tristate DATA_TRI[15:0] (
    .A(dTriA),
    .Y(dTriY),
    .S(dTriEn)
);

// Registers to control relays.
DffEn SCAN_RELAY (
    .D(scanD),
    .Q(scanQ),
    .S(scanEn),
    .clk(clk),
    .rstn(rstn)
);
DffEn SPI_RELAY (
    .D(spiD),
    .Q(spiQ),
    .S(spiEn),
    .clk(clk),
    .rstn(rstn)
);

// Register used to control if MCU is paused.
DffEn PAUSE_EN (
    .D(pauseD),
    .Q(pauseQ),
    .S(pauseEn),
    .clk(clk),
    .rstn(rstn)
);

//////////////////////////
// -- Connects/Logic -- //
//////////////////////////

// Determine next state/update state.
Mux2 M0[2:0] (
    .A({2'b10, jtagTMS}), 
    .B({2'b00, ~jtagTMS}), 
    .S(stateQ[0]), 
    .Y(nextState00X)
);
Mux2 M1[2:0] (
    .A({1'b0, ~jtagTMS, 1'b0}),
    .B(nextState00X),
    .S(stateQ[1]),
    .Y(nextState0XX)
);
Mux2 M2[2:0] (
    .A({1'b0, ~jtagTMS, 1'b0}), 
    .B({1'b1, jtagTMS, 1'b0}), 
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

// Compute signals based on state machine.
assign inUpdate  = (stateQ[2] & stateQ[1] & ~stateQ[0]);
assign inDShift  = (~stateQ[2] & stateQ[1] & ~stateQ[0]);
assign getStatus = (~stateQ[2] & ~stateQ[1] & stateQ[0]) & ~jtagTMS;

// Determine instruction register input (shift vs status).
Mux2 M4[1:0] (
    .A({isPaused, isBooted}), // status
    .B({iRegQ[0], jtagTDI}),  // shifted
    .S(getStatus),
    .Y(nextIRegBits)
);
assign iRegD = {{iRegQ[6:1]} & {6{~getStatus}}, nextIRegBits};

// Determine data register input/enable (shift vs overwrite vs nothing).
Mux2 M5[15:0] (
    .A({dRegQ[14:0], jtagTDI}),
    .B(sramData),
    .S(inDShift),
    .Y(dRegD)
);
assign dRegEn = inDShift;

// Compute signals based on command/data input.
assign is3Bit    = ~(|iRegQ[7:3]);
assign inControl = isPaused & isBooted;

// Handle RAM access registers/controls.
assign addrD  = dRegQ;
assign addrEn = (is3Bit & ~iRegQ[2] & ~iRegQ[1] & iRegQ[0]) & inUpdate;
assign dTriA  = dRegQ;
assign dTriEn = (is3Bit & ~iRegQ[2] & iRegQ[1] & iRegQ[0]) 
                & inUpdate 
                & inControl;

// Handle relay controls.
assign scanD  = (is3Bit & iRegQ[2] & ~iRegQ[1] & ~iRegQ[0]) & inControl;
assign scanEn = inUpdate;
assign spiD   = (is3Bit & iRegQ[2] & ~iRegQ[1] & iRegQ[0]) & inControl;
assign spiEn  = inUpdate;

// Handle pausing/running of MCU.
assign pauseD  = iRegQ[0];
assign pauseEn = (is3Bit & iRegQ[2] & iRegQ[1]) & inUpdate & isBooted;

// Set JTAG pin output (output from direct registers).
Mux2 M6 (
    .A(dRegQ[15]),
    .B(iRegQ[7]),
    .S(inDShift),
    .Y(jtagTDO)
);

// Set SRAM outputs.
assign sramAddr = addrQ;
assign sramData = dTriY;
assign sramWr   = iRegQ[0];
assign sramEn   = (is3Bit & ~iRegQ[2] & iRegQ[1]) & inUpdate & inControl;

// Set relay outputs.
assign enScanRelay = scanQ;
assign enSPIRelay = spiQ;

// Set 'paused' output (for pausing MCU/giving JTAG control).
assign enPaused = pauseQ;

endmodule
