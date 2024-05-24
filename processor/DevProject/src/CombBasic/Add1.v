/*
 * Add1.v
 * 
 * "Simple 1-bit full adder"
 */
module Add1 (
    // Input signals.
    input  A,
    input  B,
	 input  I,
    
	 // Output signals.
	 output S,
	 output O
);

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Computed signals common to both output computations.
wire aXorB;

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Compute intermediate signals.
assign aXorB = A ^ B;

//------------------------------------------------------------------------------
// Compute basic outputs.
assign S = aXorB ^ I;
assign O = (A & B) | (aXorB & I);

endmodule
