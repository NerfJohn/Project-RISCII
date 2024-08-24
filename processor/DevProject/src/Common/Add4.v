/*
 * Add4.v
 *
 * "Adder for two 4-bit values (+carry in)"
 */
module Add4 (
	// Adder Operands.
	input  [3:0]  A,
	input  [3:0]  B,
	
	// Carry Connections.
	input         I,
	
	// Computed Result.
	output [3:0]  S
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Adder wires.
wire [2:0] addBaseA, addBaseB, addBaseS;
wire [2:0] addBaseI, addBaseO;
wire       addCapA, addCapB, addCapS;
wire       addCapI;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Adder- mix of carry/no carry adders to get desired "no-carry" multi bit add.
AddCout1 ADD_BASE[2:0] (
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
assign addBaseA = A[2:0];
assign addBaseB = B[2:0];
assign addBaseI = {addBaseO[1:0], I};
assign addCapA  = A[3];
assign addCapB  = B[3];
assign addCapI  = addBaseO[2];

//------------------------------------------------------------------------------
// Drive adder output.
assign S = {addCapS, addBaseS};

endmodule
