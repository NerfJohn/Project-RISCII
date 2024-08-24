/*
 * AddCout1.v
 *
 * "1-bit adder with carry out"
 */
module AddCout1 (
	// Adder Operands.
	input         A,
	input         B,
	
	// Carry Connections.
	input         I,
	output        O,
	
	// Computed Result.
	output        S
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Computed controls (based on operands).
wire        xorAB;

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute operand based controls.
assign xorAB = A ^ B;

//------------------------------------------------------------------------------
// Drive full adder outputs.
assign S = xorAB ^ I;
assign O = (A & B) | (xorAB & I);

endmodule
