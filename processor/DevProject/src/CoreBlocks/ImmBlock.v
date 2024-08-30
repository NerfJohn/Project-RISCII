/*
 * ImmBlock.v
 *
 * "Generates immediate value based on given opcode"
 */
module ImmBlock (
	// Input instruction connection.
	input  [7:0]  i_instrImm,
	input  [3:0]  i_instrOpcode,
	
	// Output immediate connection.
	output [15:0] o_genImm
);

/*
 * Generates 16-bit immediate based on opcode and instruction's immediate field.
 *
 * Opcode is used to determine the primary size and signedness of immediate to
 * cast it into a 16-bit value. Opcode also used for additional modifers (eg
 * shift left by 1). In general, generates int5, int6, and int8 values.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Computed controls based on opcode.
wire        isSize6, isSize8;
wire        doShift;

// Base selection wires.
wire [15:0] smallerImm, baseImm;

// Length resolution wires.
wire [15:0] immSized;
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute opcode based controls.
assign isSize6 = ~i_instrOpcode[3] & ~i_instrOpcode[1];
assign isSize8 = ~i_instrOpcode[2] &  i_instrOpcode[1] & ~i_instrOpcode[0];
assign doShift = ~i_instrOpcode[3];

//------------------------------------------------------------------------------
// Compute base immediate.
Mux2 M0[15:0] (
	.A({{10{i_instrImm[4]}}, i_instrImm[5:0]}), // 6-bit imm? generate int6
	.B({{11{i_instrImm[4]}}, i_instrImm[4:0]}), // No?        generate int5
	.S(isSize6),
	.Y(smallerImm)
);
Mux2 M1[15:0] (
	.A({{8{i_instrImm[7]}}, i_instrImm}),       // 8-bit imm? generate int8
	.B(smallerImm),                             // No?        process small imms
	.S(isSize8),
	.Y(baseImm)
);

//------------------------------------------------------------------------------
// Drive generated immediate (shifting for control opcodes).
Mux2 M2[15:0] (
	.A({baseImm[14:0], 1'b0}), // Shift? return base left shifted by 1 bit
	.B(baseImm),               // No?    return base
	.S(doShift),
	.Y(o_genImm)
);

endmodule
