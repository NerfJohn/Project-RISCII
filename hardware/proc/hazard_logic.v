// Hazard logic- processor's "flight deck" manager.
module hazard_logic(src1, src2, wregEX, enFileEX, useAluEX, wregMEM, enFileMEM, doFwd, doStall);

// I/Os.
input enFileEX, useAluEX, enFileMEM;
input[2:0] src1, src2, wregEX, wregMEM;
output doFwd, doStall;

// Intermediate wires.
wire hzdSrc1EX, hzdSrc2EX, hzdSrc1MEM, hzdSrc2MEM;

// Pre-computation: Determine if sources match registers to be written.
assign hzdSrc1EX = enFileEX & ~((src1[2] ^ wregEX[2]) | (src1[1] ^ wregEX[1]) | (src1[0] ^ wregEX[0]));
assign hzdSrc2EX = enFileEX & ~((src2[2] ^ wregEX[2]) | (src2[1] ^ wregEX[1]) | (src2[0] ^ wregEX[0]));
assign hzdSrc1MEM = enFileMEM & ~((src1[2] ^ wregMEM[2]) | (src1[1] ^ wregMEM[1]) | (src1[0] ^ wregMEM[0]));
assign hzdSrc2MEM = enFileMEM & ~((src2[2] ^ wregMEM[2]) | (src2[1] ^ wregMEM[1]) | (src2[0] ^ wregMEM[0]));

// Determine output action: stall for any hazard, unless able to forward.
assign doStall = (hzdSrc1EX & ~useAluEX) | hzdSrc2EX | hzdSrc1MEM | hzdSrc2MEM;
assign doFwd = hzdSrc1EX & useAluEX;

endmodule