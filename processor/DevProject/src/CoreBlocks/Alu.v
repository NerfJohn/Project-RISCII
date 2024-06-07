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
wire [15:0] shiftLhs,    shiftS;
wire [3:0]  shiftRhs;
wire [1:0]  shiftOpcode;

// Wires related to the adder computations.
wire [15:0] adderA, adderB, adderS;
wire [15:0] adderI, adderO;

// Wires related to "set" computations.
wire [7:0]  setUpperByte;
wire [15:0] setS;

// Nets used to determine result of ALU.
wire [15:0] opcode01X, opcode11X;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// 16-bit barrel shifter- used for shift operations (and sub-op flags).
Shifter16 SHIFTER (
	// Operand inputs.
	.i_lhs(shiftLhs),
	.i_rhs(shiftRhs),
	
	// Control input.
	.i_opcode(shiftOpcode),
	
	// Result of shifting.
	.o_result(shiftS)
);

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
// Compute "shift" operations.
assign shiftLhs    = i_src1;
assign shiftRhs    = i_src2[3:0];
assign shiftOpcode = {i_opcode[0], ~i_flagBit};

//------------------------------------------------------------------------------
// Compute "arithmetic" operations.
assign adderA = i_src1 ^ {16{i_opcode[0]}};  // For SUB: invert A with NOT...
assign adderB = i_src2;
assign adderI = {adderO[14:0], i_opcode[0]}; // ...and add 1

//------------------------------------------------------------------------------
// Compute "set" operations.
Mux2 M0[7:0] (
	.A(i_src1[7:0]),
	.B({8{i_src2[7]}}),
	.S(i_flagBit),
	.Y(setUpperByte)
);
assign setS = {setUpperByte, i_src2[7:0]};

//------------------------------------------------------------------------------
// Select the result requested via opcode.
Mux2 M1[15:0] (
	.A(setS),
	.B(xorGateS),
	.S(i_opcode[0]),
	.Y(opcode01X)
);
Mux2 M2[15:0] (
	.A(orGateS),
	.B(andGateS),
	.S(i_opcode[0]),
	.Y(opcode11X)
);
Mux4 M3[15:0] (
	.A(adderS),
	.B(opcode01X),
	.C(shiftS),
	.D(opcode11X),
	.S(i_opcode[2:1]),
	.Y(o_result)
);

endmodule
