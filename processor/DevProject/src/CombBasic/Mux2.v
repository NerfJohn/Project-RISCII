/*
 * Mux2.v
 * 
 * "Simple 2 multiplexer- basic building block for many circuits"
 */
module Mux2 (
    // Input signals.
    input  A,
    input  B,
    
    // Control signal.
    input  S,
    
    // Output signal.
    output Y
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Basic computation- implements "Y = (S) ? A : B;".
assign Y = (A & S) | (B & ~S);

endmodule
