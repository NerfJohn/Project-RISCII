/*
 * CoreCtrl.v
 *
 * "Satellite status/controls for the uP's core"
 */
module CoreCtrl (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// Reported Info connections.
	input  [15:0] i_reportSP,
	input  [14:0] i_reportPC,
	input         i_reportHLT,
	
	// Output Control connections.
	output        o_doPause,
	output        o_intOVF,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * Implements various controls and values related to the uP's core.
 *
 * Primarily provides "pause" control accessible by the core and a "stack
 * overflow detection" feature. Also gives glimpse into PC and stack pointer
 * registers (namely as SW debuggin utilities).
 *
 * |SW Addr|HW Addr|Register Name |
 * |-------|-------|--------------|
 * |0x8000 |0xC000 |CCTRL_CTRL    |
 * |0x8002 |0xC001 |CCTRL_SETPOINT|
 * |0x8004 |0xC002 |CCTRL_PC      |
 * |0x8006 |0xC003 |CCTRL_SP
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Compute control wires (based on mem address).
wire        isCtrlAddr, isSetpointAddr;

// Control setting wires.
wire        overflowD, overflowQ, overflowEn;
wire        pauseD, pauseQ, pauseEn;

// Overflow setpoint wires.
wire [15:0] setpointD, setpointQ;
wire        setpointEn;

// Comparator/Interrupt wires.
wire [15:0] addA, addB, addS;
wire        addI, addO;
wire        intD, intQ, intEn;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Control Settings- various read/write settings to control core options.
DffSynchEn OVERFLOW_EN (
	.D(overflowD),
	.Q(overflowQ),
	.S(overflowEn),
	.clk(i_clk),
	.rstn(i_rstn)
);
DffSynchEn PAUSE_EN (
	.D(pauseD),
	.Q(pauseQ),
	.S(pauseEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Overflow Setpoint- value to compare with to determine if stack overflowed.
DffSynchEn SETPOINT[15:0] (
	.D(setpointD),
	.Q(setpointQ),
	.S(setpointEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Stack Comparator- detects if stack has overflowed an asserts INT signal.
AddCout16 COMPARATOR (
	.A(addA),
	.B(addB),
	.I(addI),
	.O(addO),
	.S(addS)
);
DffSynchEn INT (
	.D(intD),
	.Q(intQ),
	.S(intEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute address based controls.
assign isCtrlAddr     = ~i_memAddr[1] & ~i_memAddr[0]; // ...00
assign isSetpointAddr = ~i_memAddr[1] &  i_memAddr[0]; // ...01

//------------------------------------------------------------------------------
// Handle control settings inputs.
assign overflowD  = i_memDataIn[1];                         // Overflow Enable
assign overflowEn = isCtrlAddr & i_memWrEn;
assign pauseD     = i_memDataIn[0] | i_reportHLT;           // Do Pause
assign pauseEn    = (isCtrlAddr & i_memWrEn) | i_reportHLT;

//------------------------------------------------------------------------------
// Handle setpoint inputs.
assign setpointD  = i_memDataIn;
assign setpointEn = isSetpointAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Handle comparator/interrupt inputs.
assign addA  = ~i_reportSP;    // using as comparator
assign addB  = setpointQ;
assign addI  = 1'b1;           // using as comparator
assign intD  = (|addS) & addO; // "SP < setpoint"
assign intEn = overflowQ;

//------------------------------------------------------------------------------
// Drive data output based on given address.
Mux4 M0[15:0] (
	.C({14'b00000000000000, overflowQ, pauseQ}), // Address 00? Read controls
	.D(setpointQ),                               // Address 01? Read setpoint
	.E({i_reportPC, 1'b0}),                      // Address 10? Read PC
	.F(i_reportSP),                              // Address 11? Read SP
	.S(i_memAddr),
	.Y(o_memDataOut)
);

//------------------------------------------------------------------------------
// Drive control outputs.
assign o_doPause = pauseQ;
assign o_intOVF  = intQ & overflowQ; // Disable interrupt w/ enable bit

endmodule
