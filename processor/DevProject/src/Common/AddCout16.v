/*
 * AddCout16.v
 *
 * "Adder for two 16-bit values with carry in/out"
 */
module AddCout16 (
	// Adder Operands.
	input  [15:0]  A,
	input  [15:0]  B,
	
	// Carry Connections.
	input          I,
	output         O,
	
	// Computed Result.
	output [15:0]  S
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Adder wires.
wire [15:0] addA, addB, addI, addO, addS;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Adder- mix of carry/no carry adders to get desired "no-carry" multi bit add.
AddCout1 ADDER[15:0] (
	.A(addA),
	.B(addB),
	.I(addI),
	.O(addO),
	.S(addS)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Handle Adder inputs.
assign addA = A;
assign addB = B;
assign addI = {addO[14:0], I};

//------------------------------------------------------------------------------
// Drive adder output.
assign S = addS;
assign O = addO[15];

endmodule
