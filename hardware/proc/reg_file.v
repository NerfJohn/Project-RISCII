// Register File- Top of the (memory) World.
module reg_file(raddr1, raddr2, waddr, wdata, wr, rdata1, rdata2, clk, rstn);

// Clk and rstn.
input clk, rstn;

// I/Os.
input wr;
input[2:0] raddr1, raddr2, waddr;
input[15:0] wdata;
output[15:0] rdata1, rdata2;

// Intermediate wires.
wire skipBypass1, skipBypass2;
wire en0, en1, en2, en3, en4, en5, en6, en7;
wire[7:0] regEns;
wire[15:0] regStoredWord1, regStoredWord2;
wire[31:0] regStoredPair1, regStoredPair2;
wire[127:0] regQs;

// 16-bit registers instantiation.
reg_16b iREGS0[7:0] (.D(wdata), .Q(regQs), .en({en7, en6, en5, en4, en3, en2, en1, en0}), .clk(clk), .rstn(rstn));

// Compute bypass logic.
assign skipBypass1 = (raddr1[2] ^ waddr[2]) | (raddr1[1] ^ waddr[1]) | (raddr1[0] ^ waddr[0]);
assign skipBypass2 = (raddr2[2] ^ waddr[2]) | (raddr2[1] ^ waddr[1]) | (raddr2[0] ^ waddr[0]);

// Decode the write address into the enable logic.
assign en0 = ~waddr[2] & ~waddr[1] & ~waddr[0];
assign en1 = ~waddr[2] & ~waddr[1] & waddr[0];
assign en2 = ~waddr[2] & waddr[1] & ~waddr[0];
assign en3 = ~waddr[2] & waddr[1] & waddr[0];
assign en4 = waddr[2] & ~waddr[1] & ~waddr[0];
assign en5 = waddr[2] & ~waddr[1] & waddr[0];
assign en6 = waddr[2] & waddr[1] & ~waddr[0];
assign en7 = waddr[2] & waddr[1] & waddr[0];

// Determine the first read address data.
mux4 iMUXES0[31:0] (.A(regQs[31:0]), .B(regQs[63:32]), .C(regQs[95:64]), .D(regQs[127:96]), .sel(raddr1[2:1]), .out(regStoredPair1));
mux2 iMUXES1[15:0] (.A(regStoredPair1[31:16]), .B(regStoredPair1[15:0]), .sel(raddr1[0]), .out(regStoredWord1));
mux2 iMUXES2[15:0] (.A(regStoredWord1), .B(wdata), .sel(skipBypass1), .out(rdata1));

// Determine the second read address data.
mux4 iMUXES3[31:0] (.A(regQs[31:0]), .B(regQs[63:32]), .C(regQs[95:64]), .D(regQs[127:96]), .sel(raddr2[2:1]), .out(regStoredPair2));
mux2 iMUXES4[15:0] (.A(regStoredPair2[31:16]), .B(regStoredPair2[15:0]), .sel(raddr2[0]), .out(regStoredWord2));
mux2 iMUXES5[15:0] (.A(regStoredWord2), .B(wdata), .sel(skipBypass2), .out(rdata2));

endmodule