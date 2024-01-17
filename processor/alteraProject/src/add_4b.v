module add_4b(A, B, Cin, S, Cout);

// 4-bit ripple carry adder.

// I/Os.
input Cin;
input[3:0] A, B;
output Cout;
output[3:0] S;

// Intermediate wires.
wire[2:0] carries; // 3 carry signals

// Full adder instantiations.
add_1b iADDS0[3:0] (.A(A), .B(B), .Cin({carries, Cin}),
						.S(S), .Cout({Cout,carries}));
						
endmodule
