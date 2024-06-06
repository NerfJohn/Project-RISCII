/*
 * Core.v
 *
 * "Top level of core/software-executing logic of RISCII microprocessor"
 */
module Core (
    // Memory bus connection (for instructions/data- always enabled).
    output [15:0] o_memAddr,
    inout  [15:0] io_memData,
    output        o_memWr,
    
    // MCU state signals (pause source + multi-cycle pause).
    output        o_doPause,
    input         i_startPause,
    output        o_nowPaused,
    input         i_isBooted,
    
    // Common signals.
    input         i_clk,
    input         i_rstn
);

/*
 * TODO- desc. module
 */

//////////////////////////////////
// -- Internal Signals/Wires -- //
//////////////////////////////////

// Wires related to core's memory controller.
wire [14:0] memIAddr;
wire        memIBusy;
wire [14:0] memDAddr;
wire [15:0] memDData;
wire        memDWr,   memDSwp,  memDEn;
wire        memDRead, memDDone;
wire [15:0] memCoreAddr;
wire        memCoreWr;

// Wires related to Program Counter (PC) register (word addressable).
wire [14:0] pcD,    pcQ;
wire        pcRstn;
wire [14:0] pcIncA, pcIncY, pcIncCarry;

// Wires related to register implementing "pipline" of core.
wire [31:0] pipeD, pipeQ;
wire        pipeEn;
wire        pipeClear;
wire [14:0] pipePc2;
wire [15:0] pipeInstr;

// Computed signals for pipeline hazard detection/resolution.
wire doStallPc, doClearPipe, doFreezePipe;

// Wires related to control signals for executing instructions.
wire [3:0] ctrlOpcode;
wire       ctrlImmFlag, ctrlArithFlag, ctrlShiftFlag;
wire       isPSE,       isSWP,         isMemWriter,   isMemInstr;
wire       isSrcDst,    isSrcVal,      isFileEn;
wire       isUseImm;
wire [2:0] isAluOp;
wire       isAluFlag;

// Wires related to register file.
wire [2:0]  regRAddr1, regRAddr2;
wire [15:0] regRData1, regRData2;
wire [2:0]  regWAddr;
wire [15:0] regWData;
wire        regWEn;

// Wires related to immediate block.
wire [3:0]  immOpcode;
wire [7:0]  immSrc;
wire [15:0] immResult;

// Wires/Nets related to ALU.
wire [15:0] aluSrc1, aluSrc2, aluResult;
wire [2:0]  aluOpcode;
wire        aluFlag;

// Nets related to determining which result to save to register file.
wire [15:0] wbResult;

///////////////////////////////////////
// -- Functional Blocks/Instances -- //
///////////////////////////////////////

//------------------------------------------------------------------------------
// Core's memory controller- handles accesses/schedule for instructions/data.
CoreMemController MEM_CTRL (
    // Instruction memory inputs (implied enable/read- also word addressable).
    .i_iAddr(memIAddr),
	 
	 // Instruction memory status signals.
	 .o_iIsBusy(memIBusy),
	 
	 // Data memory inputs.
	 .i_dAddr(memDAddr),
	 .i_dData(memDData),
	 .i_dWr(memDWr),
	 .i_dSwp(memDSwp),
	 .i_dEn(memDEn),
	 
	 // Data memory status signals.
	 .o_dIsReading(memDRead),
	 .o_dIsDone(memDDone),
    
    // Unified memory interface (data for writing, implied enable).
    .o_coreAddr(memCoreAddr),
    .io_coreData(io_memData), // direct connect- verilog inout limitations
    .o_coreWr(memCoreWr),
	 
	 // Common signals.
	 .i_clk(i_clk),
	 .i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Program Counter (PC) Register + incrementor- start of any instruction.
DffSynch PC[14:0] (
    .D(pcD),
    .Q(pcQ),
    .clk(i_clk),
    .rstn(pcRstn)
);
Add1 PC_INC[14:0] ( // TODO- using as incrementor- refactor?
    .A(pcIncA),
    .B(15'b000000000000000),
    .I({pcIncCarry[13:0], 1'b1}), 
    .S(pcIncY),
    .O(pcIncCarry)
);

//------------------------------------------------------------------------------
// Pipeline register- split's core into FETCH and EXECUTE/MEMORY stages.
DffSynchEn PIPE_REG[31:0] (
	.D(pipeD),
	.Q(pipeQ),
	.S(pipeEn),
	.clk(i_clk),
	.rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Control logic- determines controls signals to execute instructions.
ControlLogic CTRL_LOGIC (
	// Inputs used to determine signals.
	.i_opcode(ctrlOpcode),
	.i_immFlag(ctrlImmFlag),
	.i_arithFlag(ctrlArithFlag),
	.i_shiftFlag(ctrlShiftFlag),
	
	// Computed signals related to instruction(s) detection.
	.o_isPSE(isPSE),
	.o_isSWP(isSWP),
	.o_isMemWriter(isMemWriter),
	.o_isMemInstr(isMemInstr),
	
	// Computed signals related to register file.
	.o_isSrcDst(isSrcDst),
	.o_isSrcVal(isSrcVal),
	.o_isFileEn(isFileEn),
	
	// Computed signals related to selecting signals.
	.o_isUseImm(isUseImm),
	
	// Computed signals related to ALU.
	.o_isAluOp(isAluOp),
	.o_isAluFlag(isAluFlag)
);

//------------------------------------------------------------------------------
// Register file- "color palette" of the core processor.
RegFile REG_FILE (
	// Read addresses.
	.i_rAddr1(regRAddr1),
	.i_rAddr2(regRAddr2),
	
	// Write inputs/controls (synchronous).
	.i_wData(regWData),
	.i_wAddr(regWAddr),
	.i_wEn(regWEn),
	
	// Result of reads (asynchronous).
	.o_rData1(regRData1),
	.o_rData2(regRData2),
	
	// Common signals.
	.i_clk(i_clk),
	.i_rstn(i_rstn)
);

//------------------------------------------------------------------------------
// Immediate Block- generates immediate from instruction.
ImmBlock IMM_BLOCK (
	// Inputs fields from instruction.
	.i_opcode(immOpcode),
	.i_immSrc(immSrc),
	
	// Generated output immediate.
	.o_result(immResult)
);

//------------------------------------------------------------------------------
// Arithmetic Logic Unit (ALU)- main computational circuit of core.
Alu ALU (
	// Operand inputs.
	.i_src1(aluSrc1),
	.i_src2(aluSrc2),
	
	// Control inputs.
	.i_opcode(aluOpcode),
	.i_flagBit(aluFlag),
	
	// Computed output.
	.o_result(aluResult)
);

///////////////////////////////////////////
// -- Connections/Combinational Logic -- //
///////////////////////////////////////////

//------------------------------------------------------------------------------
// Connect core parts to memory controller.
assign memIAddr = pcQ;
assign memDAddr = aluResult[15:1];
assign memDData = regRData2;
assign memDWr   = isMemWriter;
assign memDSwp  = isSWP;
assign memDEn   = isMemInstr;

//------------------------------------------------------------------------------
// Determine next PC address to attempt to read.
Mux2 M0[14:0] (
	.A(pcQ),
	.B(pcIncY),
	.S(doStallPc),
	.Y(pcD)
);
assign pcRstn = i_rstn & i_isBooted; // wait at 0x0000 while booting
assign pcIncA = pcQ;

//------------------------------------------------------------------------------
// Handle pipeline register logic- transitioning data between stages.
assign pipeD     = {doClearPipe, pcIncY, io_memData};
assign pipeEn    = ~doFreezePipe & i_isBooted;                    // NOP boot
assign pipeClear = pipeQ[31];
assign pipePc2   = pipeQ[30:16];
assign pipeInstr = {pipeQ[15:12] & {4{~pipeClear}}, pipeQ[11:0]}; // forces NOP

//------------------------------------------------------------------------------
// Compute hazard signals.
assign doStallPc    = i_startPause | isPSE | isMemInstr;
assign doClearPipe  = i_startPause | memIBusy;
assign doFreezePipe = isPSE | (isMemInstr & ~memDDone);

//------------------------------------------------------------------------------
// Get control signals from opcode/instruction flags.
assign ctrlOpcode    = pipeInstr[15:12]; // opcode for all instructions
assign ctrlImmFlag   = pipeInstr[5];     // data instructions' "immediate" flag
assign ctrlArithFlag = pipeInstr[4];     // SHR instruction's "arithmetic" flag
assign ctrlShiftFlag = pipeInstr[8];     // LBI instruction's "shift" flag

//------------------------------------------------------------------------------
// Handle register file logic- being readable and writable.
Mux2 M1[2:0] (
	.A(pipeInstr[11:9]),
	.B(pipeInstr[8:6]),
	.S(isSrcDst),
	.Y(regRAddr1)
);
Mux2 M2[2:0] (
	.A(pipeInstr[11:9]),
	.B(pipeInstr[2:0]),
	.S(isSrcVal),
	.Y(regRAddr2)
);
assign regWData = wbResult;
assign regWAddr = pipeInstr[11:9];
assign regWEn   = isFileEn & (~isMemInstr | memDRead); // write once for LDR/SWP

//------------------------------------------------------------------------------
// Generate immediate value for instruction use.
assign immOpcode = pipeInstr[15:12];
assign immSrc    = pipeInstr[7:0];

//------------------------------------------------------------------------------
// Calculate result of desired operation.
assign aluSrc1   = regRData1;
Mux2 M3[15:0] (
	.A(immResult),
	.B(regRData2),
	.S(isUseImm),
	.Y(aluSrc2)
);
assign aluOpcode = isAluOp;
assign aluFlag   = isAluFlag;

//------------------------------------------------------------------------------
// Determine which result to use for writeback.
Mux2 M4[15:0] (
	.A(io_memData),
	.B(aluResult),
	.S(memDRead),
	.Y(wbResult)
);

//------------------------------------------------------------------------------
// Connect memory controller to core's interface to the MCU.
assign o_memAddr = memCoreAddr;
assign o_memWr   = memCoreWr;

//------------------------------------------------------------------------------
// Determine pause controls based on core and pause details.
assign o_doPause   = isPSE;
assign o_nowPaused = i_startPause & (pipeClear | isPSE); // clear or PSE

endmodule
