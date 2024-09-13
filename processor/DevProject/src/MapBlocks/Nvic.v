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
	input         i_intEXH,
	input         i_intTM0,
	input         i_intTM1,
	input         i_intEXL,
	
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
wire [10:0] enableD, enableQ;
wire        enableEn;

// Flag register wires.
wire        doSetFlag;
wire [10:0] inFlags;
wire [10:0] flagD, flagQ;
wire        flagEn;

// Encoder wires.
wire        encIn1, encIn2, encIn3, encIn4, encIn5;
wire        encIn6, encIn7, encIn8, encIn9, encIn10;
wire        encIn11;
wire [3:0]  encVal;

// Compute data wires (based on mem address).
wire [15:0] readAddr0X;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Enable register- determines which flags can cause an interrupt.
DffSynchEn ENABLE[10:0] (
	.D(enableD),
	.Q(enableQ),
	.S(enableEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Flag register- tracks the status of each interrupt in the system.
DffSynchEn FLAGS[10:0] (
	.D(flagD),
	.Q(flagQ),
	.S(flagEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Priority Encoder- encodes inputs into highest value signal.
NvicEncoder ENCODER (
	// Decoded connections.
	.i_in1(encIn1),
	.i_in2(encIn2),
	.i_in3(encIn3),
	.i_in4(encIn4),
	.i_in5(encIn5),
	.i_in6(encIn6),
	.i_in7(encIn7),
	.i_in8(encIn8),
	.i_in9(encIn9),
	.i_in10(encIn10),
	.i_in11(encIn11),
	
	// Encoded connections.
	.o_encVal(encVal)
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
assign enableD  = i_memDataIn[11:1];
assign enableEn = isEnableAddr & i_memWrEn;

//------------------------------------------------------------------------------
// Handle flag register inputs.
assign doSetFlag = i_intOVF |
                   i_intEXH |
						 i_intTM0 |
						 i_intTM1 |
						 i_intEXL;
assign inFlags   = {i_intOVF,
						  i_intEXH,
						  1'b0,                                // TODO- implement
						  i_intTM0,
						  i_intTM1,
						  5'b00000,                            // TODO- implement
						  i_intEXL};
Mux2 M0[10:0] (
	.A(i_memDataIn[11:1]),                                // Data Wr? Use data
	.B(flagQ | inFlags),                                  // No?      Mesh flags
 	.S(isFlagAddr & i_memWrEn),
	.Y(flagD)
);
assign flagEn    = doSetFlag | (isFlagAddr & i_memWrEn);

//------------------------------------------------------------------------------
// Handle encoder inputs.
assign encIn1  = enableQ[0]  & flagQ[0];  // low ext. pin (EXL)- lowest priority
assign encIn2  = enableQ[1]  & flagQ[1];  // TODO- implement
assign encIn3  = enableQ[2]  & flagQ[2];  // TODO- implement
assign encIn4  = enableQ[3]  & flagQ[3];  // TODO- implement
assign encIn5  = enableQ[4]  & flagQ[4];  // TODO- implement
assign encIn6  = enableQ[5]  & flagQ[5];  // TODO- implement
assign encIn7  = enableQ[6]  & flagQ[6];  // TODO- implement
assign encIn8  = enableQ[7]  & flagQ[7];  // TODO- implement
assign encIn9  = enableQ[8]  & flagQ[8];  // TODO- implement
assign encIn10 = enableQ[9]  & flagQ[9];  // high ext. pin (EXH)
assign encIn11 = enableQ[10] & flagQ[10]; // overflow (OVF)- highest priority

//------------------------------------------------------------------------------
// Drive data output based on given address.
Mux2 M1[15:0] (
	.A({4'b0000, flagQ, 1'b0}),   // Addr x1? read flag register
	.B({4'b0000, enableQ, 1'b0}), // Addr x0? read enable register
	.S(i_memAddr[0]),
	.Y(readAddr0X)
);
assign o_memDataOut = readAddr0X & {16{~i_memAddr[1]}};

//------------------------------------------------------------------------------
// Drive interrupt (to core).
assign o_intCode = encVal;
assign o_intEn   = |encVal; // "0000" has no corresponding interrupt

endmodule
