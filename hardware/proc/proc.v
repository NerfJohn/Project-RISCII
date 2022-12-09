module proc (	inDataD, inAddrD, isRdD, startReqD, outDataD, reqFinishD,
				inDataI, inAddrI, startReqI, outDataI, reqFinishI,
				sigHalt, sigFirstWord,
				clk, rstn);

// Std. clk and rstn signals.
input clk, rstn;

// Data Memory I/O.
input reqFinishD;
input [63:0] outDataD;
output isRdD, startReqD;
output [15:0] inAddrD;
output [63:0] inDataD;

// Instruction Memory I/O.
input reqFinishI;
input [63:0] outDataI;
output startReqI;
output [15:0] inAddrI;
output [63:0] inDataI;

// Test Signal outputs.
output sigHalt;
output [15:0] sigFirstWord;

// TEMP: Set outputs to steady values.
assign isRdD = 0;
assign startReqD = 0;
assign inAddrD = 0;
assign inDataD = 0;
assign startReqI = 0;
assign inAddrI = 0;
assign inDataI = 0;
assign sigHalt = 1; // TEMP: automatically halt
assign sigFirstWord = 0;

endmodule