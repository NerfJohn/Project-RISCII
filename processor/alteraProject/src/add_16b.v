module add_16b(src1, src2, cin, sum, cout);

// 16-bit ripple carry adder.

// I/Os.
input cin;
input[15:0] src1, src2;
output cout;
output[15:0] sum;

// Intermediate wires.
wire[14:0] carries; // 15 carry signals

// Full adder instantiations.
add_1b iADDS0[15:0] (.A(src1), .B(src2), .Cin({carries, cin}),
						.S(sum), .Cout({cout,carries}));
						
endmodule
