/*
 * Mu4.v
 * 
 * "Four-way multiplexer- shorthand for multiple Mux2 instances"
 */
module Mux4 (
    // Input signals.
    input       A,
    input       B,
	 input       C,
	 input       D,
    
    // Control signal.
    input [1:0] S,
    
    // Output signal.
    output      Y
);

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Nets between computed signals.
wire fromAB, fromCD;

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute the four-way selection- assumes A-D == 0-3.
Mux2 M0 (
	.A(B),
	.B(A),
	.S(S[0]),
	.Y(fromAB)
);
Mux2 M1 (
	.A(D),
	.B(C),
	.S(S[0]),
	.Y(fromCD)
);
Mux2 M2 (
	.A(fromCD),
	.B(fromAB),
	.S(S[1]),
	.Y(Y)
);

endmodule
