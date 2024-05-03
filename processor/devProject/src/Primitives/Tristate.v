/*
 * Tristate.sv
 * 
 * "Unidirectional Pass Gate- passes driving input or high impedance signal"
 */
module Tristate (
    // Tristate I/O.
    input  A,
    output Y,
    
    // Control signal.
    input  S
);

// Select between driving output with input or not drive net at all.
assign Y = (S) ? A : 1'bZ;

endmodule
