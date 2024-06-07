/*
 * Shifter16.v
 *
 * "Barrel shifter specialized for core's specific shift instructions"
 */
module Shifter16 (
	// Operand inputs.
	input  [15:0] i_lhs,
	input  [3:0]  i_rhs,
	
	// Control input.
	input  [1:0]  i_opcode,
	
	// Result of shifting.
	output [15:0] o_result
);

/*
 * Shifter16 - implements 3/4 types of shifting for 16-bit numbers.
 *
 * Implements shift operations for RISCII's ALU. Shift operations are inferred
 * as an extra opcode (apart from ALU's opcode). See below for available
 * operations.
 *
 * == Shift Operations ==
 * opcode | operation
 * -------+----------
 * 2'b00  | no shift (used to avoid shifting by certain digits)
 * 2'b01  | left logical shift
 * 2'b10  | right arithmetic shift
 * 2'b11  | right logical shift
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to computing each layer's select signals.
wire [1:0]  selL1, selL2, selL3, selL4;

// Wires related to the result of each shift layer.
wire [15:0] shiftL1, shiftL2, shiftL3, shiftL4;

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// First layer of shifting- 1 bit shifts.
assign selL1 = i_opcode & {2{i_rhs[0]}};
Mux4 M0[15:0] (
	.A(i_lhs),                    // No Shift
	.B({i_lhs[14:0], 1'b0}),      // Shift Logic Left
	.C({i_lhs[15], i_lhs[15:1]}), // Shift Arith Right
	.D({1'b0, i_lhs[15:1]}),      // Shift Logic Right
	.S(selL1),
	.Y(shiftL1)
);

//------------------------------------------------------------------------------
// Second layer of shifting- 2 bit shifts.
assign selL2 = i_opcode & {2{i_rhs[1]}};
Mux4 M1[15:0] (
	.A(shiftL1),                           // No Shift
	.B({shiftL1[13:0], 2'b00}),            // Shift Logic Left
	.C({{2{shiftL1[15]}}, shiftL1[15:2]}), // Shift Arith Right
	.D({2'b00, shiftL1[15:2]}),            // Shift Logic Right
	.S(selL2),
	.Y(shiftL2)
);

//------------------------------------------------------------------------------
// Third layer of shifting- 4 bit shifts.
assign selL3 = i_opcode & {2{i_rhs[2]}};
Mux4 M2[15:0] (
	.A(shiftL2),                           // No Shift
	.B({shiftL2[11:0], 4'b0000}),          // Shift Logic Left
	.C({{4{shiftL2[15]}}, shiftL2[15:4]}), // Shift Arith Right
	.D({4'b0000, shiftL2[15:4]}),          // Shift Logic Right
	.S(selL3),
	.Y(shiftL3)
);

//------------------------------------------------------------------------------
// Fourth layer of shifting- 8 bit shifts.
assign selL4 = i_opcode & {2{i_rhs[3]}};
Mux4 M3[15:0] (
	.A(shiftL3),                           // No Shift
	.B({shiftL3[7:0], 8'b00000000}),       // Shift Logic Left
	.C({{8{shiftL3[15]}}, shiftL3[15:8]}), // Shift Arith Right
	.D({8'b00000000, shiftL3[15:8]}),      // Shift Logic Right
	.S(selL4),
	.Y(shiftL4)
);

//------------------------------------------------------------------------------
// Return shifted value.
assign o_result = shiftL4;

endmodule
