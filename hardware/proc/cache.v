// (Direct Mapped) Cache- recent transaction storage.
module cache(inIndex, inBlock, inTag, inDirty, wr, outTag, outBlock, outValid, outDirty, clk, rstn);

// Std. clk and rstn.
input clk, rstn;

// I/Os.
input inDirty, wr;
input[4:0] inIndex;
input[7:0] inTag;
input[63:0] inBlock;
output outValid, outDirty;
output[7:0] outTag;
output[63:0] outBlock;

// Intermediate signals.
wire a, b, c, d, e; // Index address- [43210] == [abcde]
wire [31:0] en;
wire [73:0] unIdx0, unIdx1;
wire [147:0] dblIdx0, dblIdx1, dblIdx2, dblIdx3;
wire [591:0] octIdx0, octIdx1, octIdx2, octIdx3;

// Cache indicies instantiations (32 indicies and metadata entries).
reg_index iREGS0[31:0] (.D({inTag, 1'b1, inDirty, inBlock}), .Q({octIdx3, octIdx2, octIdx1, octIdx0}), .en(en), .clk(clk), .rstn(rstn));

// Decode the write address for enable.
assign a = inIndex[4];
assign b = inIndex[3];
assign c = inIndex[2];
assign d = inIndex[1];
assign e = inIndex[0];
assign en[0] = wr & ~a & ~b & ~c & ~d & ~e;
assign en[1] = wr & ~a & ~b & ~c & ~d & e;
assign en[2] = wr & ~a & ~b & ~c & d & ~e;
assign en[3] = wr & ~a & ~b & ~c & d & e;
assign en[4] = wr & ~a & ~b & c & ~d & ~e;
assign en[5] = wr & ~a & ~b & c & ~d & e;
assign en[6] = wr & ~a & ~b & c & d & ~e;
assign en[7] = wr & ~a & ~b & c & d & e;
assign en[8] = wr & ~a & b & ~c & ~d & ~e;
assign en[9] = wr & ~a & b & ~c & ~d & e;
assign en[10] = wr & ~a & b & ~c & d & ~e;
assign en[11] = wr & ~a & b & ~c & d & e;
assign en[12] = wr & ~a & b & c & ~d & ~e;
assign en[13] = wr & ~a & b & c & ~d & e;
assign en[14] = wr & ~a & b & c & d & ~e;
assign en[15] = wr & ~a & b & c & d & e;
assign en[16] = wr & a & ~b & ~c & ~d & ~e;
assign en[17] = wr & a & ~b & ~c & ~d & e;
assign en[18] = wr & a & ~b & ~c & d & ~e;
assign en[19] = wr & a & ~b & ~c & d & e;
assign en[20] = wr & a & ~b & c & ~d & ~e;
assign en[21] = wr & a & ~b & c & ~d & e;
assign en[22] = wr & a & ~b & c & d & ~e;
assign en[23] = wr & a & ~b & c & d & e;
assign en[24] = wr & a & b & ~c & ~d & ~e;
assign en[25] = wr & a & b & ~c & ~d & e;
assign en[26] = wr & a & b & ~c & d & ~e;
assign en[27] = wr & a & b & ~c & d & e;
assign en[28] = wr & a & b & c & ~d & ~e;
assign en[29] = wr & a & b & c & ~d & e;
assign en[30] = wr & a & b & c & d & ~e;
assign en[31] = wr & a & b & c & d & e;

// Determine the read cache line (and metadata).
mux4 iMUXES0[591:0] (.A(octIdx0), .B(octIdx1), .C(octIdx2), .D(octIdx3), .sel(inIndex[4:3]), .out({dblIdx3, dblIdx2, dblIdx1, dblIdx0}));
mux4 iMUXES1[147:0] (.A(dblIdx0), .B(dblIdx1), .C(dblIdx2), .D(dblIdx3), .sel(inIndex[2:1]), .out({unIdx1, unIdx0}));
mux2 iMUXES2[73:0] (.A(unIdx1), .B(unIdx0), .sel(inIndex[0]), .out({outTag, outValid, outDirty, outBlock}));

endmodule