module dut_soc (pwrOn, clk, firstWord, halt);

// Inputs.
input pwrOn;

// Outputs.
output clk, halt;
output [15:0] firstWord;

// Intermediate signals for generator.
logic rstn;

// Intermediate signals for iMem.
logic startReqI, reqFinishI;
logic [15:0] inAddrI;
logic [63:0] inDataI, outDataI;

// Intermediate signals for dMem.
logic startReqD, reqFinishD, isRdD;
logic [15:0] inAddrD;
logic [63:0] inDataD, outDataD;

// "Middle-men" signals for outputs.
logic haltLogic;
logic [15:0] firstWordLogic;

// Clk/rst signal generator.
clk_rst generator (.pwrOn(pwrOn), .clk(clk), .rstn(rstn));

// Instruction Memory (ROM).
mem_64k #(.NON_VOLATILE(1)) iMem(.inData(inDataI), .inAddr(inAddrI), .isRd(1'b1 /* always reading */), .startReq(startReqI), 
									.outData(outDataI), .reqFinish(reqFinishI), 
									.clk(clk), .rstn(rstn));

// Data Memory (RAM).
mem_64k #(.NON_VOLATILE(0)) dMem(.inData(inDataD), .inAddr(inAddrD), .isRd(isRdD), .startReq(startReqD), 
									.outData(outDataD), .reqFinish(reqFinishD), 
									.clk(clk), .rstn(rstn));
									
// Processor device under test (DUT).
proc dut (.inDataD(inDataD), .inAddrD(inAddrD), .isRdD(isRdD), .startReqD(startReqD), .outDataD(outDataD), .reqFinishD(reqFinishD),
			.inDataI(inDataI), .inAddrI(inAddrI), .startReqI(startReqI), .outDataI(outDataI), .reqFinishI(reqFinishI),
			.sigHalt(haltLogic), .sigFirstWord(firstWordLogic),
			.clk(clk), .rstn(rstn));

// Connect "middle-men" to outputs.
assign halt = haltLogic;
assign firstWord = firstWordLogic;

endmodule