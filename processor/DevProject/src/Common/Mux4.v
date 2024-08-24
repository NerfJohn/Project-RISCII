/*
 * Mux4.v
 * 
 * "Four way multiplexer- for more complex selection"
 */
module Mux4 (
    // Input signals.
    input        C,
    input        D,
	 input        E,
	 input        F,
    
    // Control signal.
    input [1:0]  S,
    
    // Output signal.
    output       Y
);

////////////////////////////////////////////////////////////////////////////////
// -- Internal Signals/Wires -- //
////////////////////////////////////////////////////////////////////////////////

// Intermediate mux wires.
wire       muxCD, muxEF; 

////////////////////////////////////////////////////////////////////////////////
// -- Connections/Comb Logic -- //
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Drive output based on selection.
Mux2 M0 (
	.A(D),     // Select x1? Pass D (01)
	.B(C),     // No (x0)?   Pass C (00)
	.S(S[0]),
	.Y(muxCD)
);
Mux2 M1 (
	.A(F),     // Select x1? Pass F (11)
	.B(E),     // No (x0)?   Pass E (10)
	.S(S[0]),
	.Y(muxEF)
);
Mux2 M2 (
	.A(muxEF), // Select 1x? Process E (10) and F (11)
	.B(muxCD), // No (0x)?   Process C (00) and D (01)
	.S(S[1]),
	.Y(Y)
);

endmodule
