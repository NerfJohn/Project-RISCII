/*
 * Add3.v
 *
 * "Adder for two 3-bit values (+carry in)"
 */
module Add3 (
	// Adder Operands.
	input  [2:0]  A,
	input  [2:0]  B,
	
	// Carry Connections.
	input         I,
	
	// Computed Result.
	output [2:0]  S
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Adder wires.
wire [1:0] addBaseA, addBaseB, addBaseS;
wire [1:0] addBaseI, addBaseO;
wire       addCapA, addCapB, addCapS;
wire       addCapI;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Adder- mix of carry/no carry adders to get desired "no-carry" multi bit add.
AddCout1 ADD_BASE[1:0] (
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
assign addBaseA = A[1:0];
assign addBaseB = B[1:0];
assign addBaseI = {addBaseO[0], I};
assign addCapA  = A[2];
assign addCapB  = B[2];
assign addCapI  = addBaseO[1];

//------------------------------------------------------------------------------
// Drive adder output.
assign S = {addCapS, addBaseS};

endmodule
