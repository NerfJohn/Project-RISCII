// DFF with enable- typical of most registers.
module dff_en(D, Q, en, clk, rstn);

// clk/rstn signals.
input clk, rstn;

// I/Os.
input D, en;
output reg Q;

// Intermediate wires.
wire nextD;

// DFF instantiation.
dff iDFF0 (.D(nextD), .Q(Q), .clk(clk), .rstn(rstn));

// MUX instantiation (as enable logic).
mux2 iMUX0 (.A(D), .B(Q), .sel(en), .out(nextD));

endmodule