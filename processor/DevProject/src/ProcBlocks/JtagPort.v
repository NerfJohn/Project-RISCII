/*
 * JtagPort.v
 *
 * "Circuit to parse JTAG signals into requests and control signals"
 */
module JtagPort (
	// JTAG pin connections.
	input         i_TCK,
	input         i_TMS,
	input         i_TDI,
	output        o_TDO,
	
	// Current state of the uP.
	input         i_isBooted,
	input         i_isPaused,
	
	// Runtime memory connection.
	input  [15:0] i_memDataIn,
	output [15:0] o_memAddr,
	output [15:0] o_memDataOut,
	output        o_memWr,
	output        o_memEn,
	
	// Access enable connections.
	output        o_spiAccess,
	output        o_scanAccess,
	
	// Pause sequence trigger.
	output        o_doPause,
	
	// Common signals.
	input         i_rstn
);

/*
 * Parses JTAG pin inputs into internal/external outputs.
 *
 * Implements available JTAG commands, asserting control signals used by the uP.
 * Also uses inputs to provide the host insight into the uP's status and the
 * contents of the directly accessible shift registers.
 *
 * Design Notes:
 * 1) All registers update on TCK- NOT the uP system clock
 * 2) Internal control registers updates in UPDATE state
 * 3) SCAN/SPI control signals intended for enable + TDO selection
 * 4) Input pause signal should reflect true PAUSED state (vs paused in BOOTING)
 */
 
////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// State Machine wires.
wire [2:0]  stateD, stateQ;
wire [2:0]  nextState1XX, nextState0XX;
wire [2:0]  nextState10X, nextState00X;

// Computed controls (based on state machine).
wire        inDSHFT, inUPDATE;
wire        loadStatus;

// Shift Registers wires.
wire [7:0]  cmdD, cmdQ;
wire [15:0] dataD, dataQ;
wire        dataEn;

// Computed controls (based on shift inputs + state).
wire        is3BitCmd;
wire        exeAddrCmd, exeMemCmd, exeAccessCmd, exeStateCmd;

// Address register wires.
wire [15:0] addrD, addrQ;
wire        addrEn;

// Access enable registers wires.
wire        spiSelD, spiSelQ, spiSelEn;
wire        scanSelD, scanSelQ, scanSelEn;

// Pause trigger register wires.
wire        pauseD, pauseQ, pauseEn;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// State Machine- controlled by JTAG pins, controls overall port.
DffAsynch STATE[2:0] (
	.D(stateD),
	.Q(stateQ),
	.clk(i_TCK),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Shift registers- directly affected by JTAG pin inputs.
DffAsynch CMD[7:0] (
	.D(cmdD),
	.Q(cmdQ),
	.clk(i_TCK),
	.rstn(i_rstn)
);
DffAsynchEn DATA[15:0] (
	.D(dataD),
	.Q(dataQ),
	.S(dataEn),
	.clk(i_TCK),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Address register- stores address for (runtime) memory accesses.
DffAsynchEn ADDR[15:0] (
	.D(addrD),
	.Q(addrQ),
	.S(addrEn),
	.clk(i_TCK),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Access enable registers- stores options used to control TDO output source.
DffAsynchEn SPI_SEL (
	.D(spiSelD),
	.Q(spiSelQ),
	.S(spiSelEn),
	.clk(i_TCK),
	.rstn(i_rstn)
);
DffAsynchEn SCAN_SEL (
	.D(scanSelD),
	.Q(scanSelQ),
	.S(scanSelEn),
	.clk(i_TCK),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Pause trigger register- stores driving signal to pause uP.
DffAsynchEn PAUSE (
	.D(pauseD),
	.Q(pauseQ),
	.S(pauseEn),
	.clk(i_TCK),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Determine next state of JTAG/state machine.
Mux2 M0[2:0] (
	.A({1'b0, ~i_TMS, 1'b0}), // D-SEL  (101)? To D-SHFT (010) or IDLE (000)
	.B({1'b1, i_TMS, 1'b0}),  // I-SHFT (100)? To I-SHFT (100) or UPDATE (110)
	.S(stateQ[0]),
	.Y(nextState10X)
);
Mux2 M1[2:0] (
	.A({1'b0, 1'b0, 1'b0}),   // UPDATE (11X)? To IDLE (000)
	.B(nextState10X),         // No     (10X)? Process bit 0
	.S(stateQ[1]),
	.Y(nextState1XX)
);
Mux2 M2[2:0] (
	.A({1'b1, 1'b0, i_TMS}),  // I-SEL (001)? To I-SHFT (100) or D-SEL (101)
	.B({1'b0, 1'b0, ~i_TMS}), // IDLE  (000)? To I-SEL  (001) or IDLE  (000)
	.S(stateQ[0]),
	.Y(nextState00X)
);
Mux2 M3[2:0] (
	.A({1'b0, ~i_TMS, 1'b0}), // D-SHFT (01X)? To D-SHFT (010) or IDLE (000)
	.B(nextState00X),         // No     (00X)? Process bit 0
	.S(stateQ[1]),
	.Y(nextState0XX)
);
Mux2 M4[2:0] (
	.A(nextState1XX),         // Bit 2 set? Process bit 1 as 1XX
	.B(nextState0XX),         // No?        Process bit 1 as 0XX
	.S(stateQ[2]),
	.Y(stateD)
);

//------------------------------------------------------------------------------
// Compute state machine based controls.
assign inDSHFT    = ~stateQ[2] &  stateQ[1];
assign inUPDATE   =  stateQ[2] &  stateQ[1]  & ~stateQ[0];
assign loadStatus = ~stateQ[2] & ~stateQ[1]  &  stateQ[0] & ~i_TMS;

//------------------------------------------------------------------------------
// Handle shift registers- shifted vs set inputs.
Mux2 M5[7:0] (
	.A({6'b000000, i_isPaused, i_isBooted}), // Enter ISHFT? Load uP Status
	.B({cmdQ[6:0], i_TDI}),                  // No?          Left shift in TDI
	.S(loadStatus),
	.Y(cmdD)
);
Mux2 M6[15:0] (
	.A(i_memDataIn),
	.B({dataQ[14:0], i_TDI}),
	.S(inUPDATE),
	.Y(dataD)
);
assign dataEn = inDSHFT | exeMemCmd;

//------------------------------------------------------------------------------
// Compute shift based controls.
assign is3BitCmd    = ~(|cmdQ[7:3]);
assign exeAddrCmd   = is3BitCmd & ~cmdQ[2] & ~cmdQ[1] & cmdQ[0] & inUPDATE;
assign exeMemCmd    = is3BitCmd & ~cmdQ[2] &  cmdQ[1] & inUPDATE & i_isPaused;
assign exeAccessCmd = is3BitCmd &  cmdQ[2] & ~cmdQ[1] & inUPDATE & i_isPaused;
assign exeStateCmd  = is3BitCmd &  cmdQ[2] &  cmdQ[1] & inUPDATE & i_isBooted;

//------------------------------------------------------------------------------
// Set address as commanded.
assign addrD  = dataQ;
assign addrEn = exeAddrCmd;

//------------------------------------------------------------------------------
// Record access control selection.
assign spiSelD   = exeAccessCmd & cmdQ[0];  // Enable for access command...
assign spiSelEn  = inUPDATE;                // ...updates for any command
assign scanSelD  = exeAccessCmd & ~cmdQ[0];
assign scanSelEn = inUPDATE;

//------------------------------------------------------------------------------
// Record "PauseUnpause" actions.
assign pauseD  = cmdQ[0];
assign pauseEn = exeStateCmd;

//------------------------------------------------------------------------------
// Source TDO from shift registers.
Mux2 M7 (
	.A(dataQ[15]),
	.B(cmdQ[7]),
	.S(inDSHFT),
	.Y(o_TDO)
);

//------------------------------------------------------------------------------
// Set runtime memory outputs.
assign o_memAddr    = addrQ;
assign o_memDataOut = dataQ;
assign o_memWr      = cmdQ[0];   // read vs. write cmd bit
assign o_memEn      = exeMemCmd;

//------------------------------------------------------------------------------
// Set access enable outputs.
assign o_spiAccess  = inDSHFT & spiSelQ;
assign o_scanAccess = inDSHFT & scanSelQ;

//------------------------------------------------------------------------------
// Pause uP as commanded.
assign o_doPause = pauseQ;

endmodule
