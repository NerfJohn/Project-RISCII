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
 * TODO- desc.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////
	
// Length resolution wires.
wire [15:0] immSized;
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// TODO- implement.
assign immSized = {{8{i_instrImm[7]}}, i_instrImm};

//------------------------------------------------------------------------------
// Drive generated immediate (shifting for control opcodes).
Mux2 M0[15:0] (
	.A({immSized[14:0], 1'b0}),
	.B(immSized),
	.S(~i_instrOpcode[3]),
	.Y(o_genImm)
);

endmodule
