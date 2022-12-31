// 16-bit barrel shifter.
module shifter_16b(in, cnt, op, out);

/* Opcode Definitions:
	00: No shift
	01: Left Logical
	10: Right Logical
	11: Right Arithmetic
*/

// I/Os.
input[1:0] op;
input[3:0] cnt;
input[15:0] in;
output[15:0] out;

// Intermediate signals.
wire[15:0] shift1, shift2, shift3;

// Mux instantiations.
mux4 iMUXES0[15:0] (.A(in), .B({in[14:0], 1'h0}), .C({1'h0, in[15:1]}),							// 1-bit shift phase.
					.D({in[15], in[15:1]}), .sel(op & {2{cnt[0]}}), .out(shift1));
					
mux4 iMUXES1[15:0] (.A(shift1), .B({shift1[13:0], 2'h0}), .C({2'h0, shift1[15:2]}), 			// 2-bit shift phase.
					.D({{2{shift1[15]}}, shift1[15:2]}), .sel(op & {2{cnt[1]}}), .out(shift2));
					
mux4 iMUXES2[15:0] (.A(shift2), .B({shift2[11:0], 4'h0}), .C({4'h0, shift2[15:4]}), 			// 4-bit shift phase.
					.D({{4{shift2[15]}}, shift2[15:4]}), .sel(op & {2{cnt[2]}}), .out(shift3));
					
mux4 iMUXES3[15:0] (.A(shift3), .B({shift3[7:0], 8'h0}), .C({8'h0, shift3[15:8]}), 				// 8-bit shift phase.
					.D({{8{shift3[15]}}, shift3[15:8]}), .sel(op & {2{cnt[3]}}), .out(out));

endmodule