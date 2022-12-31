// Immediate Block- calculates immediate for later use.
module imm_block(opcode, rawImm, finImm);

/* Immediate Definitions (based on opcode):
	6-bit: 01XX
	9-bit: 00XX | X010
	5-bit: else
	(All immediates are sign extended.)
*/

// I/Os.
input[3:0] opcode;
input[8:0] rawImm;
output[15:0] finImm;

// Intermediate wires.
wire shortExtBit, use6Imm, use9Imm;
wire[10:0] upperBits;

// Determine select signals for immediate length.
assign use6Imm = ~opcode[3] & opcode[2];
assign use9Imm = ~(opcode[3] | opcode[2]) | ~(opcode[2] | ~opcode[1] | opcode[0]);

// Pre-computation: 5-bit vs. 6-bit extension bit (shorter immediates' extension).
mux2 iMUX0 (.A(rawImm[5]), .B(rawImm[4]), .sel(use6Imm), .out(shortExtBit));

// Compute the upper 11 bits (sext(9-bit) vs. sext(shorter immediate)).
mux2 iMUXES1[10:0] (.A({{7{rawImm[8]}}, rawImm[8:5]}), .B({11{shortExtBit}}), .sel(use9Imm), .out(upperBits));

// Contatenate result.
assign finImm = {upperBits, rawImm[4:0]};


endmodule