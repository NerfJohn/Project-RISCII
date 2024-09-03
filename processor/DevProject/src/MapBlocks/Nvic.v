/*
 * Nvic.v
 *
 * "Memory mapped controls for prioritized interrupts"
 */
module Nvic (
	// Memory Map connections.
	input  [1:0]  i_memAddr,
	input  [15:0] i_memDataIn,
	input         i_memWrEn,
	output [15:0] o_memDataOut,
	
	// Input flag connections.
	input         i_intOVF,
	
	// Output interrupt connections.
	output [3:0]  o_intCode,
	output        o_intEn,
	
	// Common signals.
	input         i_clk,
	input         i_rstn
);

/*
 * TODO- desc once all funcs are made.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Computed controls (based on mem address).
wire        isEnableAddr, isFlagAddr;

// Enable register wires.
wire        enableD, enableQ;
wire        enableEn;

// Flag register wires.
wire        doSetFlag;
wire        flagD, flagQ;
wire        flagEn;

// Compute data wires (based on mem address).
wire [15:0] readAddr0X;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Enable register- determines which flags can cause an interrupt.
DffSynchEn ENABLE (
	.D(enableD),
	.Q(enableQ),
	.S(enableEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Flag register- tracks the status of each interrupt in the system.
DffSynchEn FLAGS (
	.D(flagD),
	.Q(flagQ),
	.S(flagEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute address based controls.
assign isEnableAddr   = ~i_memAddr[1] & ~i_memAddr[0]; // ...00
assign isFlagAddr     = ~i_memAddr[1] &  i_memAddr[0]; // ...01

//------------------------------------------------------------------------------
// Handle enable register inputs.
assign enableD  = i_memDataIn[11];
assign enableEn = isEnableAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Handle flag register inputs.
assign doSetFlag = i_intOVF;
assign flagD     = i_memDataIn[11] | i_intOVF;
assign flagEn    = doSetFlag | (isFlagAddr & i_memWrEn);

//------------------------------------------------------------------------------
// Drive data output based on given address.
Mux2 M0[15:0] (
	.A({4'b0000, flagQ, 11'b00000000000}),   // Addr x1? read flag register
	.B({4'b0000, enableQ, 11'b00000000000}), // Addr x0? read enable register
	.S(i_memAddr[0]),
	.Y(readAddr0X)
);
assign o_memDataOut = readAddr0X & {16{~i_memAddr[1]}};

//------------------------------------------------------------------------------
// Drive interrupt (to core).
assign o_intCode = 4'b1011;         // TODO- implement
assign o_intEn   = enableQ & flagQ; // TODO- implement

endmodule
