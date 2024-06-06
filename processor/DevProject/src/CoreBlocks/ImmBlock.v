/*
 * ImmBlock.v
 *
 * "Generates immediate value based on given instruction"
 */
module ImmBlock (
	// Inputs fields from instruction.
	input  [3:0]  i_opcode,
	input  [7:0]  i_immSrc,
	
	// Generated output immediate.
	output [15:0] o_result
);

/*
 * Immediate Block- extracts immediate from instructions for use.
 *
 * Can generate for 5, 6, or 8-bit, sign extended immediate (16 bits in length).
 * Can also sign generated immediate left by 1 bit (as per instruction opcode).
 * Block decodes/interprets opcodes to determine overall immediate.
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Computed signals to control the generation of the immediate.
wire do6Bit, do8Bit, do1Shift;

// Nets to compute extension bit (ie bit used for upper byte).
wire extBitShorts;
wire extBit;

// Nets to compute the two highest bits of the raw immediate.
wire [1:0] upBitsShorts;
wire [1:0] upBits;

// Nets to compute sixth bit of raw immediate.
wire sixthBit;

// Nets related to computing final immediate value.
wire [15:0] preShiftImm;

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute control signals.
assign do6Bit   = ~i_opcode[3] & ~i_opcode[1];               // opcodes 0b0X0X
assign do8Bit   = ~i_opcode[2] & i_opcode[1] & ~i_opcode[0]; // opcodes 0bX010
assign do1Shift = ~i_opcode[3];                              // opcodes 0b0XXX

//------------------------------------------------------------------------------
// Determine the immediate's upper 8 bits (ie 5 vs 6 vs 8 bit extension).
Mux2 M0 (
	.A(i_immSrc[5]),
	.B(i_immSrc[4]),
	.S(do6Bit),
	.Y(extBitShorts)
);
Mux2 M1 (
	.A(i_immSrc[7]),
	.B(extBitShorts),
	.S(do8Bit),
	.Y(extBit)
);

//------------------------------------------------------------------------------
// Determine the top of the low byte (8 bit literal vs extended 5 or 6 bit).
Mux2 M2[1:0] (
	.A({2{i_immSrc[5]}}),
	.B({2{i_immSrc[4]}}),
	.S(do6Bit),
	.Y(upBitsShorts)
);
Mux2 M3[1:0] (
	.A(i_immSrc[7:6]),
	.B(upBitsShorts),
	.S(do8Bit),
	.Y(upBits)
);

//------------------------------------------------------------------------------
// Determine the sixth bit (literal for 6 and 8 bit, extended for 5 bit).
Mux2 M4 (
	.A(i_immSrc[5]),
	.B(i_immSrc[4]),
	.S(do6Bit | do8Bit),
	.Y(sixthBit)
);

//------------------------------------------------------------------------------
// Determine/output final immediate.
assign preShiftImm = {{8{extBit}}, upBits, sixthBit, i_immSrc[4:0]};
Mux2 M5[15:0] (
	.A({preShiftImm[14:0], 1'b0}),
	.B(preShiftImm),
	.S(do1Shift),
	.Y(o_result)
);

endmodule
