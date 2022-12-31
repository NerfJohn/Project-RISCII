// 1-bit full adder.
module add_1b(A, B, Cin, S, Cout);

// I/Os.
input A, B, Cin;
output S, Cout;

// Intermediate wires.
wire aXorB;

// Pre-calculation logic.
assign aXorB = A ^ B;

// Core logic.
assign S = aXorB ^ Cin;
assign Cout = (A & B) | (aXorB & Cin);

endmodule