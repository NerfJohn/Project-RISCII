/*
 * Add1.v
 *
 * "1-bit adder"
 */
module Add1 (
	// Adder Operands.
	input         A,
	input         B,
	
	// Carry Connections.
	input         I,
	
	// Computed Result.
	output        S
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Drive half adder output.
assign S = A ^ B ^ I;

endmodule
