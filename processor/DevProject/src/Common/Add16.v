/*
 * Add16.v
 *
 * "Adder for two 16-bit values (+carry in)"
 */
module Add16 (
	// Adder Operands.
	input  [15:0] A,
	input  [15:0] B,
	
	// Carry Connections.
	input         I,
	
	// Computed Result.
	output [15:0] S
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Adder wires.
wire [14:0] addBaseA, addBaseB, addBaseS;
wire [14:0] addBaseI, addBaseO;
wire        addCapA, addCapB, addCapS;
wire        addCapI;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Adder- mix of carry/no carry adders to get desired "no-carry" multi bit add.
AddCout1 ADD_BASE[14:0] (
	.A(addBaseA),
	.B(addBaseB),
	.I(addBaseI),
	.O(addBaseO),
	.S(addBaseS)
);
Add1 ADD_CAP (
	.A(addCapA),
	.B(addCapB),
	.I(addCapI),
	.S(addCapS)
);

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Handle Adder inputs.
assign addBaseA = A[14:0];
assign addBaseB = B[14:0];
assign addBaseI = {addBaseO[13:0], I};
assign addCapA  = A[15];
assign addCapB  = B[15];
assign addCapI  = addBaseO[14];

//------------------------------------------------------------------------------
// Drive adder output.
assign S = {addCapS, addBaseS};

endmodule
