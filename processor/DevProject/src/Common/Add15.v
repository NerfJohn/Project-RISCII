/*
 * Add15.v
 *
 * "Adder for two 15-bit values (+carry in)"
 */
module Add15 (
	// Adder Operands.
	input  [14:0]  A,
	input  [14:0]  B,
	
	// Carry Connections.
	input          I,
	
	// Computed Result.
	output [14:0]  S
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Adder wires.
wire [13:0] addBaseA, addBaseB, addBaseS;
wire [13:0] addBaseI, addBaseO;
wire        addCapA, addCapB, addCapS;
wire        addCapI;

////////////////////////////////////////////////////////////////////////////////
// -- Large Blocks/Instances -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Adder- mix of carry/no carry adders to get desired "no-carry" multi bit add.
AddCout1 ADD_BASE[13:0] (
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
assign addBaseA = A[13:0];
assign addBaseB = B[13:0];
assign addBaseI = {addBaseO[12:0], I};
assign addCapA  = A[14];
assign addCapB  = B[14];
assign addCapI  = addBaseO[13];

//------------------------------------------------------------------------------
// Drive adder output.
assign S = {addCapS, addBaseS};

endmodule
