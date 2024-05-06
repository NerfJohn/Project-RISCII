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

/*
 * JTAG Port State Machine:
 *
 * Small state machine to allow for shifting in instructions and data in and out
 * of the JTAG circuit. State machine always active after reset, though JTAG
 * port itself may not execute anything until MCU is booted or paused.
 *
 * Instruction register is 8 bits wide while the data register is 16 bits wide.
 *
 * IDLE   (000): to I-SEL for "TMS = 0", else IDLE
 * I-SEL  (001): to I_SHFT for "TMS = 0", else D-SEL
 * D-SEL  (101): to D-SHFT for "TMS = 0", else IDLE
 * I-SHFT (100): to I-SHFT for "TMS = 0", else UPDATE
 * D-SHFT (010): to D-SHFT for "TMS = 0", else UPDATE
 * UPDATE (110): always to IDLE
 */

/*
 * JTAG Port Commands:
 *
 * 8-bit commands that can be shifted into instruction register and executed
 * upon entering the UPDATE state. All commands have no effect when MCU is in
 * "booting" state, while all but "Pause MCU" have no effect when MCU is in
 * "running" state.
 *
 * No Operation (0x00): No change to JTAG or MCU
 * Set Address  (0x01): Saves data register value as RAM address
 * Read RAM     (0x02): Reads value at saved RAM address to data register
 * Write RAM    (0x03): Writes data register to saved RAM address
 * Access Scan  (0x04): Relays data shifting to Boundary Scan register
 * Access SPI   (0x05): Relays data shifting to EEPROM SPI pins
 * Pause MCU    (0x06): Sets MCU to "paused" state
 * Run MCU      (0x07): Sets MCU to "running" state
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
wire getStatus, inDShift;

// Instruction register wires/computed input.
wire[7:0] iRegD, iRegQ;
wire[1:0] nextIRegBits;

// Data register wires/computed input.
wire[15:0] dRegD, dRegQ;
wire dRegEn;

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
    .A({jtagTMS, 2'b10}),
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
assign getStatus = (~stateQ[2] & ~stateQ[1] & stateQ[0]) & ~jtagTMS;
assign inDShift  = (~stateQ[2] & stateQ[1] & ~stateQ[0]);

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

// TODO- quick test remove
assign sramAddr = {4'b0, iRegQ, 1'b0, stateQ};
assign sramData = dRegQ;

endmodule
