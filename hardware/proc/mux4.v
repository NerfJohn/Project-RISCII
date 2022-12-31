// 4 signal multiplexer- common component in signal processing.
module mux4 (A, B, C, D, sel, out);

/* Mux Definitions for select signal):
	00: A
	01: B
	10: C
	11: D
*/

// I/Os.
input A, B, C, D;
input[1:0] sel;
output out;

// Intermediate wires.
wire[1:0] mux1;

// Mux instantiations/logic.
mux2 iMUXES0[1:0] (.A({D,B}), .B({C,A}), .sel(sel[0]), .out(mux1));
mux2 iMUX1 (.A(mux1[1]), .B(mux1[0]), .sel(sel[1]), .out());

endmodule