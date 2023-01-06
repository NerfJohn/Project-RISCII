// Control logic- processor's main command.
module control_logic(opcode, immFlag, enFile, doHalt, useImm, aluOp, allowJmp, forceJmp, aluToPc, enMem, isRd, aluToFile);

/* Control Signals:
	enFile: Enable write to the register file.
	doHalt: Halt the processor.
	useImm: Use the immediate value over SR2 for calculation.
	aluOp: ALU opcode to select calculation (3-bits).
	allowJmp: Allow BRC instruction to potentially jump.
	forceJmp: Force jump to occur.
	aluToPc: Send the ALU output to the PC (vs. PC offset output).
	enMem: Enable memory (and send result to register file).
	isRd: Specific memory operation as a read (vs. a write).
	aluToFile: Send the ALU output to the register file (vs. PC+2).
*/

// I/Os.
input immFlag; // Flag bit used by data operations.
input[3:0] opcode;
output enFile, doHalt, useImm, allowJmp, forceJmp, aluToPc, enMem, isRd, aluToFile;
output[2:0] aluOp;

////////////////////////////////////////////////
// Central Command- Computing Control Signals //
////////////////////////////////////////////////

// Determine enable register file signal. (1XXX or 0BC0 for B | C).
assign enFile = opcode[3] | (~opcode[0] & (opcode[2] | opcode[1])); 

// Determine halt signal. (simply 0000).
assign doHalt = ~(|opcode);

// Determine use immediate signal (data w/ flag, control operation, or LBI).
assign useImm = (~opcode[3] | immFlag) | (~opcode[2] & opcode[1] & ~opcode[0]);

// Determine ALU opcode (embedded in data opcodes, or 000 for control).
assign aluOp = {3{opcode[3]}} & opcode[2:0];

// Determine allowJmp signal (just BRC's 0011).
assign allowJmp = (~opcode[3] & ~opcode[2] & opcode[1] & opcode[0]);

// Determine forceJmp signal (jump instructions).
assign forceJmp = (~opcode[3] & opcode[1] & ~opcode[0]) |
					(~opcode[3] & opcode[2] & ~opcode[1] & opcode[0]);

// Determine if ALU result should be sent to the PC.
assign aluToPc = opcode[2];

// Determine enable memory signal (also sends memory result to register file).
assign enMem = (~opcode[3] & opcode[2]) & ~(opcode[1] ^ opcode[0]);

// Determine the read/write signal.
assign isRd = ~opcode[0];

// Determine if the ALU result should be sent to the register file.
assign aluToFile = opcode[3];

endmodule