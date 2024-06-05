/*
 * Alu.v
 *
 * "Common register file- 8 words, 2 reads and 1 write per cycle"
 */
module Alu (
	// Operand inputs.
	input  [15:0] i_src1,
	input  [15:0] i_src2,
	
	// Control inputs.
	input  [2:0]  i_opcode,
	input         i_flagBit,
	
	// Computed output.
	output [15:0] o_result
);

/*
 * TODO- desc. module
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to the logic gate computations.
wire [15:0] andGateS, orGateS, xorGateS;

// Wires/Nets related to barrel shifting computations.
wire [1:0]  shiftOp1, shiftOp2, shiftOp3, shiftOp4;
wire [15:0] shiftL1,  shiftL2,  shiftL3,  shiftS;

// Wires related to the adder computations.
wire [15:0] adderA, adderB, adderS;
wire [15:0] adderI, adderO;

// Wires related to "set" computations.
wire [7:0]  setUpperByte;
wire [15:0] setS;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// 16-bit adder- used for integer addition and subtraction computations.
Add1 ADDER[15:0] (
	.A(adderA),
	.B(adderB),
	.I(adderI),
	.S(adderS),
	.O(adderO)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute "logic gate" operations.
assign andGateS = i_src1 & i_src2;
assign orGateS  = i_src1 | i_src2;
assign xorGateS = i_src1 ^ i_src2;

//------------------------------------------------------------------------------

// TODO- actually implement; for now, just add to get LDR, STR, and SWP working.
assign adderA = i_src1;
assign adderB = i_src2;
assign adderI = {adderO[14:0], 1'b0};
assign o_result = adderS;

endmodule
