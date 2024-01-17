module Tristate (A, en, Y);

// Signals.
input A, en;
output Y;

// -- Raw Logic -- //

// Core tristate- requires non-digital logic to implement.
assign Y = (en) ? A : 1'bz;

endmodule
