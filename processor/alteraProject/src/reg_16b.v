// 16-bit register (with enable)- base size of architecture.
module reg_16b(D, Q, en, clk, rstn);

// Clk and rstn.
input clk, rstn;

// I/Os.
input en;
input[15:0] D;
output[15:0] Q;

// DFF_en instantiation (16-bits).
dff_en iDFFEN[15:0] (.D(D), .Q(Q), .en(en), .clk(clk), .rstn(rstn));

endmodule