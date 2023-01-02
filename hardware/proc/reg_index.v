// index register (with enable)- used for each index of the cache.
module reg_index(D, Q, en, clk, rstn);

// Clk and rstn.
input clk, rstn;

// I/Os.
input en;
input[73:0] D;
output[73:0] Q;

// DFF_en instantiation (74-bits: {tag[7:0], valid, dirty, block[63:0]}).
dff_en iDFFEN[73:0] (.D(D), .Q(Q), .en(en), .clk(clk), .rstn(rstn));

endmodule