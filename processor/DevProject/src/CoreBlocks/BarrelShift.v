/*
 * BarrelShift.v
 *
 * "Performs various types of shifting using 'barrel shift' technique"
 */
module BarrelShift (
	// Operand connections.
	input  [15:0] i_opValue,
	input  [3:0]  i_opCount,
	
	// Operation connection.
	input  [1:0]  i_opCode,
	
	// Computed connection.
	output [15:0] o_result
);

/*
 * TODO- desc.
 */

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Convenience wires.
wire [15:0] val;
wire [3:0]  cnt;

// Intermediate stages.
wire [15:0] stg1, stg2, stg3;
	
////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Alias inputs for ease of reading/writing.
assign val = i_opValue;
assign cnt = i_opCount;

//------------------------------------------------------------------------------
// Compute first stage of shifting (1's shift).
Mux4 M0[15:0] (
	.C(val),                          // 00? No Shift
	.D({val[14:0], 1'b0}),            // 01? Left Logical
	.E({val[15], val[15:1]}),         // 10? Right Arithmetic
	.F({1'b0, val[15:1]}),            // 11? Right Logical
	.S(i_opCode & {2{cnt[0]}}),
	.Y(stg1)
);

//------------------------------------------------------------------------------
// Compute second stage of shifting (2's shift).
Mux4 M1[15:0] (
	.C(stg1),                         // 00? No Shift
	.D({stg1[13:0], 2'b00}),          // 01? Left Logical
	.E({{2{stg1[15]}}, stg1[15:2]}),  // 10? Right Arithmetic
	.F({2'b00, stg1[15:2]}),          // 11? Right Logical
	.S(i_opCode & {2{cnt[1]}}),
	.Y(stg2)
);

//------------------------------------------------------------------------------
// Compute third stage of shifting (4's shift).
Mux4 M2[15:0] (
	.C(stg2),                         // 00? No Shift
	.D({stg2[11:0], 4'b0000}),        // 01? Left Logical
	.E({{4{stg2[15]}}, stg2[15:4]}),  // 10? Right Arithmetic
	.F({4'b0000, stg2[15:4]}),        // 11? Right Logical
	.S(i_opCode & {2{cnt[2]}}),
	.Y(stg3)
);

//------------------------------------------------------------------------------
// Compute fourth stage of shifting (8's shift)- driving the result.
Mux4 M3[15:0] (
	.C(stg3),                         // 00? No Shift
	.D({stg3[7:0], 8'b00000000}),     // 01? Left Logical
	.E({{8{stg3[15]}}, stg3[15:8]}),  // 10? Right Arithmetic
	.F({8'b00000000, stg3[15:8]}),    // 11? Right Logical
	.S(i_opCode & {2{cnt[3]}}),
	.Y(o_result)
);

endmodule
