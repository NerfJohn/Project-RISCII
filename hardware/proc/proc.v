// Processor/Hardware Design (Top Level)- Instantiated by simulation script.
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

// FETCH Stage Intermediate Wires.
wire iStall;
wire[15:0] pc, pc2F, instrF, instrFChecked, newPc;

// DECODE Stage Intermediate Wires.
wire hStall, fwdD, enFileD, haltD, useImmD, allowJmpD, forceJmpD, aluToPcD, enMemD, isRdDC, aluToFileD, oldRst;
wire[2:0] src1, src2, aluOpD;
wire[3:0] clr, frz;
wire[4:0] ensCheckedD;
wire[15:0] pc2D, instrD, opnd1D, opnd2D, immD;

// EXECUTE Stage Intermediate Wires.
wire useImmE, aluToPcE, isRdE, haltE, fwdE, canJmp, doJmp, aluToFileE, allowJmpE, forceJmpE, enMemE, enFileE;
wire[1:0] ensCheckedE;
wire[2:0] aluOpE, brcCodeE, wRegE, newCode;
wire[15:0] opnd1E, opnd2E, immE, pc2E, selOp1, selOp2, aluResult, newPcResult, offResult;

// MEMORY Stage Intermediate Wires.
wire isRdM, enMemM, enFileM, haltM, dStall, aluToFileM, ensCheckedM;
wire[2:0] savedCode, wRegM;
wire[15:0] pc2M, aluResM, opnd2M, memResult;

// WRITEBACK Stage Intermediate Wires.
wire aluToFileW, enFileW, enMemW;
wire[2:0] wRegW;
wire[15:0] pc2W, aluResW, memResW, exResult, wbResult;

/////////////////
// FETCH Stage //
/////////////////

// PC register- determines fetched instruction.
mux2 iMUX0[15:0] (.A(newPcResult), .B(pc2F), .sel(doJmp), .out(newPc));
dff_en iPC[15:0] (.D(newPc), .Q(pc), .en(~(frz[3] | haltD) | doJmp), .clk(clk), .rstn(rstn));

// PC incrementor- implements natural incrementing of PC.
add_16b iINC (.src1(pc), .src2(16'h2), .cin(1'b0), .sum(pc2F), .cout(/*NC*/));

// Instruction Cache/Memory- fetches the instruction.
cache_sys iIMEM (.pAddr(pc), .pData(/*NC*/), .pIsRd(1'b1), .pEn(1'b1), .doStall(iStall), .pResult(instrF),
					.mAddr(inAddrI), .mData(inDataI), .mIsRd(/*NC*/), .mEn(startReqI), .done(reqFinishI), .mResult(outDataI),
					.clk(clk), .rstn(rstn));

// FETCH-DECODE Checkpoint.
assign instrFChecked = {instrF[15:13] & ~{3{clr[3]}}, instrF[12] | clr[3], instrF[11:0]};
dff_en iDFF0[15:0] (.D(instrFChecked), .Q(instrD), .en(~oldRst | ~(frz[2] | haltD)), .clk(clk), .rstn(rstn)); // instruction
dff_en iDFF1[15:0] (.D(pc2F), .Q(pc2D), .en(~oldRst | ~(frz[2] | haltD)), .clk(clk), .rstn(rstn)); // PC+2
dff iRSTDFF (.D(1'b1), .Q(oldRst), .clk(clk), .rstn(rstn)); // Detect reset 1 cycle after deassert (prevents phantom halt)

//////////////////
// DECODE Stage //
//////////////////

// Hazard Detection Logic- ensures pipeline integrity.
mux2 iMUX1[2:0] (.A(instrD[11:9]), .B(instrD[8:6]), .sel(~instrD[14] & instrD[13] & ~instrD[12]), .out(src1));
mux2 iMUX2[2:0] (.A(instrD[11:9]), .B(instrD[2:0]), .sel(~instrD[15]), .out(src2));
hazard_logic iHZRD (.src1(src1), .src2(src2), .wregEX(wRegE), .enFileEX(enFileE), .useAluEX(aluToFileE), 
					.wregMEM(wRegM), .enFileMEM(enFileM), .doFwd(fwdD), .doStall(hStall));

// Control Logic- manages processor to execute instructions.
control_logic iCTRL (.opcode(instrD[15:12]), .immFlag(instrD[5]), .enFile(enFileD), .doHalt(haltD),
						.useImm(useImmD), .aluOp(aluOpD), .allowJmp(allowJmpD), .forceJmp(forceJmpD), 
						.aluToPc(aluToPcD), .enMem(enMemD), .isRd(isRdDC), .aluToFile(aluToFileD));

// Register File- processor's "pallet".
reg_file iFILE (.raddr1(src1), .raddr2(src2), .waddr(wRegW), .wdata(wbResult), .wr(enFileW), .rdata1(opnd1D), 
				.rdata2(opnd2D), .clk(clk), .rstn(rstn));

// Immediate Block- generates immediate value to use.
imm_block iIMM (.opcode(instrD[15:12]), .rawImm(instrD[8:0]), .finImm(immD));

// Clear Logic- injects NOP into stage ([3:0] = DEMW checkpoints).
assign clr[3] = ~dStall & ((~iStall & doJmp) | (iStall & ~hStall & ~doJmp));
assign clr[2] = ~dStall & ((~iStall & (hStall | doJmp)) | (iStall & hStall & ~doJmp));
assign clr[1] = ~dStall & iStall & doJmp;
assign clr[0] = dStall;

// Freeze Logic- freezes instruction in stage ([3:0] = FDEM stages).
assign frz[3] = dStall | iStall | (hStall & ~doJmp);
assign frz[2] = dStall | (iStall & (hStall | doJmp)) | (~iStall & hStall & ~doJmp);
assign frz[1] = dStall | (iStall & doJmp);
assign frz[0] = dStall;

// DECODE-EXECUTE Checkpoint.
assign ensCheckedD = {aluToFileD, allowJmpD, forceJmpD, enMemD, enFileD} & ~{5{clr[2]}};
dff_en iDFF2[4:0] (.D({fwdD, useImmD, aluOpD}), .Q({fwdE, useImmE, aluOpE}), .en(~frz[1]), .clk(clk), .rstn(rstn)); // ALU signals {fwdE, useImm, aluOp}
dff_en iDFF3[3:0] (.D({aluToPcD, instrD[11:9]}), .Q({aluToPcE, brcCodeE}), .en(~frz[1]), .clk(clk), .rstn(rstn)); // Jump signals {aluToPc, brcCodes}
dff_en iDFF4[3:0] (.D({isRdDC, instrD[11:9]}), .Q({isRdE, wRegE}), .en(~frz[1]), .clk(clk), .rstn(rstn)); // MEM/WB signals {isRd, wReg}
dff_en iDFF5[63:0] (.D({opnd1D, opnd2D, immD, pc2D}), .Q({opnd1E, opnd2E, immE, pc2E}), .en(~frz[1]), .clk(clk), .rstn(rstn)); // Operands {op1, op2, imm, PC+2}
dff_en iDFF6[4:0] (.D(ensCheckedD), .Q({aluToFileE, allowJmpE, forceJmpE, enMemE, enFileE}), .en(~frz[1]), .clk(clk), .rstn(rstn)); // Enable signals {aluToFile, allowJmp, forceJmp, enMem, enFile}
dff_en iDFF7 (.D(haltD & ~clr[2] & oldRst), .Q(haltE), .en(~frz[1]), .clk(clk), .rstn(rstn)); // Passed halt signal

///////////////////
// EXECUTE Stage //
///////////////////

// Offset Adder- computes PC offset for jumping.
add_16b iOFST (.src1(immE), .src2(pc2E), .cin(1'b0), .sum(offResult), .cout(/*NC*/));

// ALU- main computational unit in the processor.
mux2 iMUX3[15:0] (.A(aluResM), .B(opnd1E), .sel(fwdE), .out(selOp1));
mux2 iMUX4[15:0] (.A(immE), .B(opnd2E), .sel(useImmE), .out(selOp2));
alu iALU (.src1(selOp1), .src2(selOp2), .op(aluOpE), .result(aluResult));

// Determine jump signal.
assign canJmp = (savedCode[2] & brcCodeE[2]) | (savedCode[1] & brcCodeE[1]) | (savedCode[0] & brcCodeE[0]);
assign doJmp = forceJmpE | (allowJmpE & canJmp);
mux2 iMUX5[15:0] (.A(aluResult), .B(offResult), .sel(aluToPcE), .out(newPcResult));

// Determine condition codes ([2:0] = nzp).
assign newCode[0] = ~aluResult[15];
assign newCode[1] = ~(|aluResult);
assign newCode[2] = aluResult[15];

// EXECUTE-MEMORY Checkpoint.
assign ensCheckedE = {enMemE, enFileE} & ~{2{clr[1]}};
dff_en iDFF8[2:0] (.D(newCode), .Q(savedCode), .en(~frz[0] & aluToFileE), .clk(clk), .rstn(rstn)); // Saved Condition Code
dff_en iDFF9[4:0] (.D({aluToFileE, isRdE, wRegE}), .Q({aluToFileM, isRdM, wRegM}), .en(~frz[0]), .clk(clk), .rstn(rstn)); // MEM/WB signals {aluToFile, isRd, wReg}
dff_en iDFF10[47:0] (.D({pc2E, aluResult, opnd2E}), .Q({pc2M, aluResM, opnd2M}), .en(~frz[0]), .clk(clk), .rstn(rstn)); // Operands {PC+2, aluResult, src2}
dff_en iDFF11[1:0] (.D(ensCheckedE), .Q({enMemM, enFileM}), .en(~frz[0]), .clk(clk), .rstn(rstn)); // Enable signals {enMem, enFile}
dff_en iDFF12 (.D(haltE & ~clr[1]), .Q(haltM), .en(~frz[0]), .clk(clk), .rstn(rstn)); // Passed halt signal

//////////////////
// MEMORY Stage //
//////////////////

// Data Cache/Memory- reads/writes data to/from RAM.
cache_sys iDMEM (.pAddr(aluResM), .pData(opnd2M), .pIsRd(isRdM), .pEn(enMemM), .doStall(dStall), .pResult(memResult),
					.mAddr(inAddrD), .mData(inDataD), .mIsRd(isRdD), .mEn(startReqD), .done(reqFinishD), .mResult(outDataD),
					.clk(clk), .rstn(rstn));

// (For Simulator: Connect to first word in data cache.)
assign sigFirstWord = iDMEM.iCACHE.iREGS0[0].Q[15:0];

// MEMORY-WRITEBACK Stage Checkpoint.
assign ensCheckedM = enFileM & ~clr[0];
dff iDFF13[4:0] (.D({aluToFileM, enMemM, wRegM}), .Q({aluToFileW, enMemW, wRegW}), .clk(clk), .rstn(rstn)); // MEM/WB signals {aluToFile, enMem, wReg}
dff iDFF14[47:0] (.D({pc2M, aluResM, memResult}), .Q({pc2W, aluResW, memResW}), .clk(clk), .rstn(rstn)); // Operands {PC+2, aluResult, memResult}
dff iDFF15 (.D(ensCheckedM), .Q(enFileW), .clk(clk), .rstn(rstn)); // Enable signals {enFile}
dff iDFF16 (.D(haltM & ~clr[0]), .Q(sigHalt), .clk(clk), .rstn(rstn)); // Passed halt signal

/////////////////////
// WRITEBACK Stage //
/////////////////////

// Determine final result to write back to register file.
mux2 iMUX6[15:0] (.A(aluResW), .B(pc2W), .sel(aluToFileW), .out(exResult));
mux2 iMUX7[15:0] (.A(memResW), .B(exResult), .sel(enMemW), .out(wbResult));

endmodule